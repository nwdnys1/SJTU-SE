from flask import Flask, render_template, request, send_from_directory
import neo4j
import json
import urllib.parse
from flask_cors import CORS

# neo4j数据库配置
uri = "bolt://localhost:7687"
user = "neo4j"
pwd = "123456"
driver = neo4j.GraphDatabase.driver(uri, auth=(user, pwd))


# 用flask展示数据

app = Flask(__name__)
CORS(app)


@app.route("/")
def index():
    return render_template("index.html")


# 返回指定文件路径的pdf文件
@app.route("/pdf/<filename>")
def pdf(filename):
    return send_from_directory("data", filename)


# 查找某一份简历的信息
@app.route("/resume/<filename>")
def resume(filename):
    with driver.session() as session:
        result = session.run(
            f"""
            MATCH (d:Document)
            WHERE d.filename = "{filename}"
            RETURN d.text
            """
        )

        res = ""
        for record in result:
            res = record["d.text"]
        return res


# 展示某个候选人的信息
@app.route("/person/<name>")
def person(name):
    with driver.session() as session:

        result = session.run(
            f"""
            MATCH (p:Person)
            WHERE p.name = "{name}"
            OPTIONAL MATCH (p)-[:POSSESSES]->(sk:Skill)
            OPTIONAL MATCH (p)-[:WORKED_AS]->(jt:Jobtitle)
            OPTIONAL MATCH (p)-[:HAS_EDUCATION]->(dg:Degree)
            OPTIONAL MATCH (p)-[:PARTICIPATED_IN]->(pr:Project)
            OPTIONAL MATCH (p)-[:LIVES_IN]->(c:Country)
            OPTIONAL MATCH (p)-[:SPEAKS]->(l:Language)
            OPTIONAL MATCH (p)-[:WORKS_IN]->(lo:Location)
            OPTIONAL MATCH (doc:Document)-[:MENTIONS]->(p)
            RETURN COLLECT(DISTINCT sk.id) AS skills,
                    COLLECT(DISTINCT jt.id) AS jobs,
                    COLLECT(DISTINCT dg.id) AS degrees,
                    COLLECT(DISTINCT pr.id) AS projects,
                    COLLECT(DISTINCT c.id) AS countries,
                    COLLECT(DISTINCT l.id) AS languages,
                    COLLECT(DISTINCT lo.id) AS locations,
                    doc.filename AS resume
            """
        )

        res = {
            "name": name,
            "skills": [],
            "jobs": [],
            "degrees": [],
            "projects": [],
            "countries": [],
            "languages": [],
            "locations": [],
            "resume": "",
        }
        for record in result:
            res["skills"] = record["skills"]
            res["jobs"] = record["jobs"]
            res["degrees"] = record["degrees"]
            res["projects"] = record["projects"]
            res["countries"] = record["countries"]
            res["languages"] = record["languages"]
            res["locations"] = record["locations"]
            res["resume"] = record["resume"]
        return json.dumps(res)


# 获取所有skills
@app.route("/skills")
def skills():
    with driver.session() as session:
        result = session.run("MATCH (s:Skill) RETURN s.id")
        skills = [record["s.id"] for record in result]
        return json.dumps(skills)


# 根据skills查找候选人
@app.route("/has_skills")
def has_skills():
    args = request.args
    skills = args.get("skills")
    # 解决url中某些符号会被转义的问题
    skills = urllib.parse.unquote(skills)
    print(skills)
    with driver.session() as session:
        result = session.run(
            f"""
            MATCH (p:Person)-[:POSSESSES]->(skill:Skill)
            WHERE skill.id IN {skills} 
            RETURN DISTINCT p.name
            """
        )

        res = []
        for record in result:
            res.append(record[0])
        return json.dumps(res)


# 获取所有jobtitles
@app.route("/jobs")
def jobtitles():
    with driver.session() as session:
        result = session.run("MATCH (j:Jobtitle) RETURN j.id")
        jobtitles = [record["j.id"] for record in result]
        return json.dumps(jobtitles)


# 根据jobtitles查找候选人
@app.route("/worked_as")
def worked_as():
    args = request.args
    jobtitles = args.get("jobtitles")
    with driver.session() as session:
        result = session.run(
            f"""
            MATCH (p:Person)-[:WORKED_AS]->(job:Jobtitle)
            WHERE job.id IN {jobtitles} 
            RETURN DISTINCT p.name
            """
        )

        res = []
        for record in result:
            res.append(record[0])
        return json.dumps(res)


# 将知识图谱转换为G6可以使用的json格式
@app.route("/graph")
def graph():
    with driver.session() as session:
        nodes = []
        nodeids = {}

        result = session.run(
            """
            MATCH (n:Person)-[:POSSESSES]->(:Skill)<-[:POSSESSES]-(m:Person)
            RETURN DISTINCT n.id, m.id
            LIMIT 300
            """
        )
        edges = []
        for i, record in enumerate(result):
            if record["n.id"] not in nodeids:
                nodes.append({"id": record["n.id"]})
            if record["m.id"] not in nodeids:
                nodes.append({"id": record["m.id"]})
            nodeids[record["n.id"]] = 1
            nodeids[record["m.id"]] = 1

            edges.append(
                {
                    "source": record["n.id"],
                    "target": record["m.id"],
                    "id": i,
                }
            )
        return json.dumps({"nodes": nodes, "edges": edges})


# 统计国家、语言、技能、学位、工作地区的人数 放入一个json中
from flask import jsonify


@app.route("/count")
def count():
    res = {}
    num = 10  # 获取出现次数最多的前10个
    # 使用一次会话获取所有数据
    with driver.session() as session:
        # 获取国家数据
        result = session.run(
            """
            MATCH (p:Person)-[:LIVES_IN]->(c:Country)
            RETURN c.id AS country_id, COUNT(p) AS count
        """
        )
        countries = []
        result = sorted(result, key=lambda x: x["count"], reverse=True)  # 按照count排序
        for record in result[:num]:
            countries.append({"id": record["country_id"], "count": record["count"]})

        # 获取语言数据
        result = session.run(
            """
            MATCH (p:Person)-[:SPEAKS]->(l:Language)
            RETURN l.id AS language_id, COUNT(p) AS count
        """
        )
        languages = []
        result = sorted(result, key=lambda x: x["count"], reverse=True)  # 按照count排序
        for record in result[:num]:
            languages.append({"id": record["language_id"], "count": record["count"]})

        # 获取技能数据
        result = session.run(
            """
            MATCH (p:Person)-[:POSSESSES]->(s:Skill)
            RETURN s.id AS skill_id, COUNT(p) AS count
        """
        )
        skills = []
        result = sorted(result, key=lambda x: x["count"], reverse=True)
        for record in result[:num]:
            skills.append({"id": record["skill_id"], "count": record["count"]})

        # 获取位置数据
        result = session.run(
            """
            MATCH (p:Person)-[:WORKS_IN]->(l:Location)
            RETURN l.id AS location_id, COUNT(p) AS count
        """
        )
        locations = []
        result = sorted(result, key=lambda x: x["count"], reverse=True)
        for record in result[:num]:
            locations.append({"id": record["location_id"], "count": record["count"]})

        # 获取工作数据
        result = session.run(
            """
            MATCH (p:Person)-[:WORKED_AS]->(j:Jobtitle)
            RETURN j.id AS job_id, COUNT(p) AS count
        """
        )
        jobs = []
        result = sorted(result, key=lambda x: x["count"], reverse=True)
        for record in result[:num]:
            jobs.append({"id": record["job_id"], "count": record["count"]})

    # 将结果整理为字典
    res = {
        "countries": countries,
        "languages": languages,
        "skills": skills,
        "locations": locations,
        "jobs": jobs,
    }

    # 使用Flask的 jsonify 直接返回JSON响应
    return jsonify(res)


if __name__ == "__main__":
    app.run(host="0.0.0.0", debug=True)

import pdfplumber
import os
from langchain_neo4j import Neo4jGraph
import neo4j
from langchain_experimental.graph_transformers import LLMGraphTransformer
from langchain_openai import ChatOpenAI
from langchain_core.documents import Document

# OpenAI API Key
os.environ["OPENAI_API_KEY"] = "sk-yvsgNIK7HzFK6EMwRjmHL8vO03B6ghxCCCTI0h1W6jiaCCov"
os.environ["OPENAI_API_BASE"] = "https://api.chatanywhere.tech/v1"

# neo4j数据库配置
uri = "bolt://localhost:7687"
user = "neo4j"
pwd = "123456"
os.environ["NEO4J_URI"] = uri
os.environ["NEO4J_USERNAME"] = user
os.environ["NEO4J_PASSWORD"] = pwd

graph = Neo4jGraph(refresh_schema=False)
driver = neo4j.GraphDatabase.driver(uri, auth=(user, pwd))


def clear_graph():
    with driver.session() as session:
        session.run("MATCH (n) DETACH DELETE n")


def extract_text_from_pdf(pdf_path):
    with pdfplumber.open(pdf_path) as pdf:
        text = ""
        for page in pdf.pages:
            text += page.extract_text()
    return text


# 读取pdf并提取文本
def read_resumes_from_folder(folder_path):
    resume_texts = {}
    for filename in os.listdir(folder_path):
        if filename.endswith(".pdf"):
            pdf_path = os.path.join(folder_path, filename)
            resume_texts[filename] = extract_text_from_pdf(pdf_path)
    # 删除所有换行符
    for filename, text in resume_texts.items():
        resume_texts[filename] = text.replace("\n", " ")

    return resume_texts

# 使用LLM对简历进行分析 提取实体和关系并存入数据库
def llm_nlp(text, filename):
    print(filename)
    llm = ChatOpenAI(temperature=0, model_name="gpt-4o-mini")

    # 定义提取的实体和关系
    nodes = [
        "Person",  # 个人
        "JobTitle",  # 职位
        "Organization",  # 组织
        "Degree",  # 学位
        "Institution",  # 教育机构
        "WorkField",  # 工作领域
        "Skill",  # 技能
        "Project",  # 项目
        "Country",  # 国家
        "Language",  # 语言
        "Location",  # 地点
    ]

    relationships = [
        ("Person", "WORKED_AS", "JobTitle"),  # 职位
        ("JobTitle", "AT", "Organization"),  # 组织
        ("Person", "HAS_EDUCATION", "Degree"),  # 学位
        ("Degree", "OBTAINED_FROM", "Institution"),  # 教育机构
        ("Person", "WORKED_IN", "WorkField"),  # 工作领域
        ("Person", "POSSESSES", "Skill"),  # 技能
        ("Person", "PARTICIPATED_IN", "Project"),  # 项目
        ("Person", "LIVES_IN", "Country"),  # 国家
        ("Person", "SPEAKS", "Language"),  # 语言
        ("Person", "WORKS_IN", "Location"),  # 地点
    ]

    properties = [
        "name",
        "age",  # 年龄
        "job_start_date",  # 开始工作日期
        "job_end_date",  # 结束工作日期
        "degree_start_date",  # 开始学位日期
        "degree_end_date",  # 结束学位日期
    ]
    
    # 将简历信息转换为图数据 注意存储文档id方便后续反向查询
    documents = [Document(page_content=text, metadata={"filename": filename})]
    llm_transformer = LLMGraphTransformer(
        llm=llm,
        allowed_nodes=nodes,
        allowed_relationships=relationships,
        node_properties=properties,
    )
    graph_documents = llm_transformer.convert_to_graph_documents(documents)

    # 将简历信息存入数据库
    graph.add_graph_documents(graph_documents, include_source=True)


# 读取简历文件夹中的所有简历
folder_path = "data"
resume_texts = read_resumes_from_folder(folder_path)

# 清空数据库
# clear_graph()

# 使用LLM对简历进行分析
for filename, text in resume_texts.items():
    llm_nlp(text, filename)

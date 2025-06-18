import mysql.connector
from mysql.connector import Error
from datetime import datetime
import pandas as pd
import matplotlib.pyplot as plt

# 数据库连接配置
config = {
    "user": "root",
    "password": "123456",
    "host": "localhost",
    "database": "sakila",
}

dw_config = {
    "user": "root",
    "password": "123456",
    "host": "localhost",
    "database": "sakila_dw",
}


def create_connection(config):
    """创建数据库连接"""
    try:
        conn = mysql.connector.connect(**config)
        return conn
    except Error as e:
        print(f"Error connecting to MySQL: {e}")
        return None


from datetime import datetime, timedelta


def populate_dim_date(conn):
    """填充日期维度表 - 从2000-01-01开始生成3650条日期数据"""
    try:
        cursor = conn.cursor()

        # 清空表
        cursor.execute("TRUNCATE TABLE dim_date")

        # 设置起始日期为2000-01-01
        start_date = datetime.strptime("2000-01-01", "%Y-%m-%d").date()

        # 生成3650天的日期数据（约10年）
        for i in range(3650):
            current_date = start_date + timedelta(days=i)

            # 计算日期属性
            date_key = int(current_date.strftime("%Y%m%d"))
            date_value = current_date
            date_short = current_date.strftime("%d/%m/%Y")
            date_medium = current_date.strftime("%b %d, %Y")
            date_long = current_date.strftime("%B %d, %Y")
            date_full = current_date.strftime("%A, %B %d, %Y")
            day_in_year = current_date.timetuple().tm_yday
            day_in_month = current_date.day
            day_abbreviation = current_date.strftime("%a")
            day_name = current_date.strftime("%A")

            # 周计算
            week_in_year = current_date.isocalendar()[1]
            week_in_month = (current_date.day - 1) // 7 + 1
            is_first_day_in_week = (
                "Y" if current_date.weekday() == 0 else "N"
            )  # 周一为每周第一天
            is_last_day_in_week = (
                "Y" if current_date.weekday() == 6 else "N"
            )  # 周日为每周最后一天

            # 月计算
            is_first_day_in_month = "Y" if current_date.day == 1 else "N"
            next_day = current_date + timedelta(days=1)
            is_last_day_in_month = "Y" if current_date.month != next_day.month else "N"
            month_number = current_date.month
            month_abbreviation = current_date.strftime("%b")
            month_name = current_date.strftime("%B")

            # 年计算
            year2 = current_date.strftime("%y")
            year4 = current_date.year

            # 季度计算
            quarter_number = (current_date.month - 1) // 3 + 1
            quarter_name = f"Q{quarter_number}"
            year_quarter = f"{year4}-{quarter_name}"
            year_month_number = current_date.strftime("%Y-%m")
            year_month_abbreviation = current_date.strftime("%Y-%b")

            # 插入数据
            cursor.execute(
                """
                INSERT INTO dim_date VALUES (
                    %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, 
                    %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s
                )
            """,
                (
                    date_key,
                    date_value,
                    date_short,
                    date_medium,
                    date_long,
                    date_full,
                    day_in_year,
                    day_in_month,
                    is_first_day_in_month,
                    is_last_day_in_month,
                    day_abbreviation,
                    day_name,
                    week_in_year,
                    week_in_month,
                    is_first_day_in_week,
                    is_last_day_in_week,
                    month_number,
                    month_abbreviation,
                    month_name,
                    year2,
                    year4,
                    quarter_name,
                    quarter_number,
                    year_quarter,
                    year_month_number,
                    year_month_abbreviation,
                ),
            )

        conn.commit()
        print("dim_date populated successfully with 3650 days starting from 2000-01-01")

    except Error as e:
        print(f"Error populating dim_date: {e}")
    finally:
        cursor.close()


def populate_dim_time(conn):
    """填充时间维度表"""
    try:
        cursor = conn.cursor()

        # 清空表
        cursor.execute("TRUNCATE TABLE dim_time")

        # 生成24小时的时间维度数据
        for hour in range(24):
            for minute in range(60):
                for second in range(60):
                    time_key = hour * 10000 + minute * 100 + second
                    time_value = f"{hour:02d}:{minute:02d}:{second:02d}"
                    hour24 = hour
                    hour12 = hour % 12 if hour % 12 != 0 else 12
                    am_pm = "AM" if hour < 12 else "PM"

                    cursor.execute(
                        """
                        INSERT INTO dim_time VALUES (
                            %s, %s, %s, %s, %s, %s, %s
                        )
                    """,
                        (time_key, time_value, hour24, hour12, minute, second, am_pm),
                    )

        conn.commit()
        print("dim_time populated successfully")

    except Error as e:
        print(f"Error populating dim_time: {e}")
    finally:
        cursor.close()


def populate_dim_staff(conn, dw_conn):
    """填充员工维度表"""
    try:
        cursor = conn.cursor(dictionary=True)
        dw_cursor = dw_conn.cursor()

        # 清空表
        dw_cursor.execute("TRUNCATE TABLE dim_staff")

        # 将主键自增值设为19
        dw_cursor.execute("ALTER TABLE dim_staff AUTO_INCREMENT = 19")

        # 从sakila获取员工数据
        cursor.execute(
            """
            SELECT s.*, a.address, a.district, a.phone, c.city 
            FROM staff s
            JOIN address a ON s.address_id = a.address_id
            JOIN city c ON a.city_id = c.city_id
        """
        )

        staffs = cursor.fetchall()

        for staff in staffs:
            dw_cursor.execute(
                """
                INSERT INTO dim_staff (
                    staff_last_update, staff_first_name, staff_last_name,
                    staff_id, staff_store_id, staff_version_number,
                    staff_valid_from, staff_valid_through, staff_active
                ) VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s)
            """,
                (
                    staff["last_update"],
                    staff["first_name"],
                    staff["last_name"],
                    staff["staff_id"],
                    staff["store_id"],
                    1,
                    "1970-01-01",
                    "2200-01-01",
                    "Yes" if staff["active"] else "No",
                ),
            )

        dw_conn.commit()
        print("dim_staff populated successfully")

    except Error as e:
        print(f"Error populating dim_staff: {e}")
    finally:
        cursor.close()
        dw_cursor.close()


def populate_dim_customer(conn, dw_conn):
    """填充客户维度表"""
    try:
        cursor = conn.cursor(dictionary=True)
        dw_cursor = dw_conn.cursor()

        # 清空表
        dw_cursor.execute("TRUNCATE TABLE dim_customer")

        # 将主键自增值设为4794
        dw_cursor.execute("ALTER TABLE dim_customer AUTO_INCREMENT = 4794")

        # 从sakila获取客户数据，关联所有相关表
        cursor.execute(
            """
            SELECT 
                c.customer_id, c.store_id, c.first_name, c.last_name, c.email, 
                c.active, c.create_date, c.last_update,
                a.address, a.address2, a.district, a.postal_code, a.phone,
                ci.city,
                co.country
            FROM customer c
            JOIN address a ON c.address_id = a.address_id
            JOIN city ci ON a.city_id = ci.city_id
            JOIN country co ON ci.country_id = co.country_id
            ORDER BY c.customer_id  # 添加排序
        """
        )

        customers = cursor.fetchall()

        for customer in customers:
            dw_cursor.execute(
                """
                INSERT INTO dim_customer (
                    customer_last_update, customer_id, customer_first_name, 
                    customer_last_name, customer_email, customer_active,
                    customer_created, customer_address, customer_district,
                    customer_postal_code, customer_phone_number, customer_city,
                    customer_country, customer_version_number, 
                    customer_valid_from, customer_valid_through
                ) VALUES (
                    %s, %s, %s, %s, %s, %s, %s, %s, %s, 
                    %s, %s, %s, %s, %s, %s, %s
                )
            """,
                (
                    customer["last_update"],
                    customer["customer_id"],
                    customer["first_name"],
                    customer["last_name"],
                    customer["email"],
                    "Yes" if customer["active"] else "No",
                    customer["create_date"].date() if customer["create_date"] else None,
                    customer["address"],
                    customer["district"],
                    customer["postal_code"],
                    customer["phone"],
                    customer["city"],
                    customer["country"],
                    1,  # version_number设为1
                    "1970-01-01",  # valid_from设为默认值
                    "2200-01-01",  # valid_through设为默认值
                ),
            )

        dw_conn.commit()
        print("dim_customer populated successfully")

    except Error as e:
        print(f"Error populating dim_customer: {e}")
        dw_conn.rollback()
    finally:
        cursor.close()
        dw_cursor.close()


def populate_dim_store(conn, dw_conn):
    """填充商店维度表"""
    try:
        cursor = conn.cursor(dictionary=True)
        dw_cursor = dw_conn.cursor()

        # 清空表
        dw_cursor.execute("TRUNCATE TABLE dim_store")

        # 将主键自增值设为8
        dw_cursor.execute("ALTER TABLE dim_store AUTO_INCREMENT = 8")

        # 从sakila获取商店数据，包括国家信息
        cursor.execute(
            """
            SELECT 
                s.*, 
                a.address, 
                a.district, 
                a.postal_code, 
                a.phone as phone_number,
                ci.city,
                co.country,
                st.first_name as manager_first_name, 
                st.last_name as manager_last_name,
                st.staff_id as manager_staff_id
            FROM store s
            JOIN address a ON s.address_id = a.address_id
            JOIN city ci ON a.city_id = ci.city_id
            JOIN country co ON ci.country_id = co.country_id
            JOIN staff st ON s.manager_staff_id = st.staff_id
        """
        )

        stores = cursor.fetchall()

        for store in stores:
            dw_cursor.execute(
                """
                INSERT INTO dim_store (
                    store_last_update,
                    store_id,
                    store_address,
                    store_district,
                    store_postal_code,
                    store_phone_number,
                    store_city,
                    store_country,
                    store_manager_staff_id,
                    store_manager_first_name,
                    store_manager_last_name,
                    store_version_number,
                    store_valid_from,
                    store_valid_through
                ) VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
            """,
                (
                    store["last_update"],  # store_last_update
                    store["store_id"],  # store_id
                    store["address"],  # store_address
                    store["district"],  # store_district
                    store["postal_code"],  # store_postal_code
                    store["phone_number"],  # store_phone_number
                    store["city"],  # store_city
                    store["country"],  # store_country
                    store["manager_staff_id"],  # store_manager_staff_id
                    store["manager_first_name"],  # store_manager_first_name
                    store["manager_last_name"],  # store_manager_last_name
                    1,  # store_version_number (默认值)
                    "1970-01-01",  # store_valid_from (默认值)
                    "2200-01-01",  # store_valid_through (默认值)
                ),
            )

        dw_conn.commit()
        print("dim_store populated successfully")

    except Error as e:
        print(f"Error populating dim_store: {e}")
    finally:
        cursor.close()
        dw_cursor.close()


def populate_dim_actor(conn, dw_conn):
    """填充演员维度表"""
    try:
        cursor = conn.cursor(dictionary=True)
        dw_cursor = dw_conn.cursor()

        # 清空表
        dw_cursor.execute("TRUNCATE TABLE dim_actor")

        # 将主键自增值设为601
        dw_cursor.execute("ALTER TABLE dim_actor AUTO_INCREMENT = 601")

        # 从sakila获取演员数据
        cursor.execute("SELECT * FROM actor")
        actors = cursor.fetchall()

        for actor in actors:
            dw_cursor.execute(
                """
                INSERT INTO dim_actor (
                    actor_last_update, actor_last_name, actor_first_name, actor_id
                ) VALUES (%s, %s, %s, %s)
            """,
                (
                    actor["last_update"],
                    actor["last_name"],
                    actor["first_name"],
                    actor["actor_id"],
                ),
            )

        dw_conn.commit()
        print("dim_actor populated successfully")

    except Error as e:
        print(f"Error populating dim_actor: {e}")
    finally:
        cursor.close()
        dw_cursor.close()


def populate_dim_film(conn, dw_conn):
    """填充电影维度表"""
    try:
        cursor = conn.cursor(dictionary=True)
        dw_cursor = dw_conn.cursor()

        # 清空表
        dw_cursor.execute("TRUNCATE TABLE dim_film")

        # 将主键自增值设为3001
        dw_cursor.execute("ALTER TABLE dim_film AUTO_INCREMENT = 3001")

        # 从sakila获取电影数据，包括类别信息和演员数量
        cursor.execute(
            """
            SELECT 
                f.*, 
                l.name as language_name, 
                IFNULL(ol.name, 'Not Applicable') as original_language_name,
                COUNT(DISTINCT fa.actor_id) as actor_count
            FROM film f
            JOIN language l ON f.language_id = l.language_id
            LEFT JOIN language ol ON f.original_language_id = ol.language_id
            LEFT JOIN film_actor fa ON f.film_id = fa.film_id
            GROUP BY f.film_id
        """
        )
        films = cursor.fetchall()

        # 获取每部电影的类别信息
        cursor.execute(
            """
            SELECT fc.film_id, c.name as category_name
            FROM film_category fc
            JOIN category c ON fc.category_id = c.category_id
        """
        )
        film_categories = cursor.fetchall()

        # 创建电影ID到类别的映射
        category_map = {}
        for fc in film_categories:
            if fc["film_id"] not in category_map:
                category_map[fc["film_id"]] = []
            category_map[fc["film_id"]].append(fc["category_name"])

        # 定义评级映射
        rating_mapping = {
            "G": "General Audiences",
            "PG": "Parental Guidance Suggested",
            "PG-13": "Parents Strongly Cautioned",
            "R": "Restricted",
            "NC-17": "No One Under 17 Admitted",
        }

        for film in films:
            # 处理special_features
            special_features = (
                film["special_features"] if film["special_features"] else []
            )
            has_trailers = "Yes" if "Trailers" in special_features else "No"
            has_commentaries = "Yes" if "Commentaries" in special_features else "No"
            has_deleted_scenes = "Yes" if "Deleted Scenes" in special_features else "No"
            has_behind_the_scenes = (
                "Yes" if "Behind the Scenes" in special_features else "No"
            )

            # 处理类别
            categories = category_map.get(film["film_id"], [])
            in_category_action = "Yes" if "Action" in categories else "No"
            in_category_animation = "Yes" if "Animation" in categories else "No"
            in_category_children = "Yes" if "Children" in categories else "No"
            in_category_classics = "Yes" if "Classics" in categories else "No"
            in_category_comedy = "Yes" if "Comedy" in categories else "No"
            in_category_documentary = "Yes" if "Documentary" in categories else "No"
            in_category_drama = "Yes" if "Drama" in categories else "No"
            in_category_family = "Yes" if "Family" in categories else "No"
            in_category_foreign = "Yes" if "Foreign" in categories else "No"
            in_category_games = "Yes" if "Games" in categories else "No"
            in_category_horror = "Yes" if "Horror" in categories else "No"
            in_category_music = "Yes" if "Music" in categories else "No"
            in_category_new = "Yes" if "New" in categories else "No"
            in_category_scifi = "Yes" if "Sci-Fi" in categories else "No"
            in_category_sports = "Yes" if "Sports" in categories else "No"
            in_category_travel = "Yes" if "Travel" in categories else "No"

            # 插入数据
            dw_cursor.execute(
                """
                INSERT INTO dim_film (
                    film_last_update, film_title, film_description,
                    film_release_year, film_language, film_original_language,
                    film_rental_duration, film_rental_rate, film_duration,
                    film_replacement_cost, film_rating_code, film_rating_text,
                    film_has_trailers, film_has_commentaries, 
                    film_has_deleted_scenes, film_has_behind_the_scenes,
                    film_in_category_action, film_in_category_animation,
                    film_in_category_children, film_in_category_classics,
                    film_in_category_comedy, film_in_category_documentary,
                    film_in_category_drama, film_in_category_family,
                    film_in_category_foreign, film_in_category_games,
                    film_in_category_horror, film_in_category_music,
                    film_in_category_new, film_in_category_scifi,
                    film_in_category_sports, film_in_category_travel,
                    film_id
                ) VALUES (
                    %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s,
                    %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s,
                    %s, %s, %s, %s, %s, %s, %s, %s, %s
                )
            """,
                (
                    film["last_update"],
                    film["title"],
                    film["description"],
                    film["release_year"],
                    film["language_name"],
                    film["original_language_name"],
                    film["rental_duration"],
                    film["rental_rate"],
                    film["length"],
                    film["replacement_cost"],
                    film["rating"],
                    rating_mapping.get(film["rating"], ""),
                    has_trailers,
                    has_commentaries,
                    has_deleted_scenes,
                    has_behind_the_scenes,
                    in_category_action,
                    in_category_animation,
                    in_category_children,
                    in_category_classics,
                    in_category_comedy,
                    in_category_documentary,
                    in_category_drama,
                    in_category_family,
                    in_category_foreign,
                    in_category_games,
                    in_category_horror,
                    in_category_music,
                    in_category_new,
                    in_category_scifi,
                    in_category_sports,
                    in_category_travel,
                    film["film_id"],
                ),
            )

        dw_conn.commit()
        print("dim_film populated successfully")

    except Error as e:
        print(f"Error populating dim_film: {e}")
    finally:
        cursor.close()
        dw_cursor.close()


def populate_film_actor_bridge(conn, dw_conn):
    """填充电影-演员桥接表"""
    try:
        cursor = conn.cursor(dictionary=True)
        dw_cursor = dw_conn.cursor()

        # 清空表
        dw_cursor.execute("TRUNCATE TABLE dim_film_actor_bridge")

        # 首先获取每部电影的演员数量
        cursor.execute(
            """
            SELECT film_id, COUNT(*) as actor_count 
            FROM film_actor 
            GROUP BY film_id
        """
        )
        film_actor_counts = {
            row["film_id"]: row["actor_count"] for row in cursor.fetchall()
        }

        # 从sakila获取电影-演员关系数据
        cursor.execute("SELECT * FROM film_actor")
        film_actors = cursor.fetchall()

        for fa in film_actors:
            # 获取电影和演员的key
            dw_cursor.execute(
                "SELECT film_key FROM dim_film WHERE film_id = %s", (fa["film_id"],)
            )
            film_key = dw_cursor.fetchone()[0]

            dw_cursor.execute(
                "SELECT actor_key FROM dim_actor WHERE actor_id = %s", (fa["actor_id"],)
            )
            actor_key = dw_cursor.fetchone()[0]

            # 计算权重因子（1/演员数量）
            actor_count = film_actor_counts.get(fa["film_id"], 1)
            weighting_factor = 1.0 / actor_count if actor_count > 0 else 0

            dw_cursor.execute(
                """
                INSERT INTO dim_film_actor_bridge (film_key, actor_key, actor_weighting_factor)
                VALUES (%s, %s, %s)
                """,
                (film_key, actor_key, weighting_factor),
            )

        dw_conn.commit()
        print("dim_film_actor_bridge populated successfully")

    except Error as e:
        print(f"Error populating dim_film_actor_bridge: {e}")
    finally:
        cursor.close()
        dw_cursor.close()


def populate_fact_rental(conn, dw_conn):
    """填充租赁事实表"""
    try:
        cursor = conn.cursor(dictionary=True)
        dw_cursor = dw_conn.cursor()

        # 清空表
        dw_cursor.execute("TRUNCATE TABLE fact_rental")

        # 从sakila获取租赁数据
        cursor.execute(
            """
            SELECT r.*, i.film_id, i.store_id, p.amount,
                   TIMESTAMPDIFF(SECOND, r.rental_date, r.return_date) AS duration_seconds
            FROM rental r
            JOIN inventory i ON r.inventory_id = i.inventory_id
            LEFT JOIN payment p ON r.rental_id = p.rental_id
            WHERE r.return_date IS NOT NULL
        """
        )

        rentals = cursor.fetchall()

        for rental in rentals:
            # 获取维度键
            # 客户维度键
            dw_cursor.execute(
                "SELECT customer_key FROM dim_customer WHERE customer_id = %s",
                (rental["customer_id"],),
            )
            customer_key = dw_cursor.fetchone()[0]

            # 员工维度键
            dw_cursor.execute(
                "SELECT staff_key FROM dim_staff WHERE staff_id = %s",
                (rental["staff_id"],),
            )
            staff_key = dw_cursor.fetchone()[0]

            # 电影维度键
            dw_cursor.execute(
                "SELECT film_key FROM dim_film WHERE film_id = %s", (rental["film_id"],)
            )
            film_key = dw_cursor.fetchone()[0]

            # 商店维度键
            dw_cursor.execute(
                "SELECT store_key FROM dim_store WHERE store_id = %s",
                (rental["store_id"],),
            )
            store_key = dw_cursor.fetchone()[0]

            # 日期和时间维度键
            rental_date = rental["rental_date"]
            rental_date_key = int(rental_date.strftime("%Y%m%d"))
            rental_time_key = (
                rental_date.hour * 10000 + rental_date.minute * 100 + rental_date.second
            )

            return_date = rental["return_date"]
            return_date_key = int(return_date.strftime("%Y%m%d"))
            return_time_key = (
                return_date.hour * 10000 + return_date.minute * 100 + return_date.second
            )

            # 计算租赁持续时间(秒)
            rental_duration = (
                rental["duration_seconds"] if rental["return_date"] else None
            )

            # 插入事实表
            dw_cursor.execute(
                """
                INSERT INTO fact_rental (
                    customer_key, staff_key, film_key, store_key,
                    rental_date_key, return_date_key, rental_time_key,
                    count_returns, count_rentals, rental_duration,
                    rental_last_update, rental_id
                ) VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
            """,
                (
                    customer_key,
                    staff_key,
                    film_key,
                    store_key,
                    rental_date_key,
                    return_date_key,
                    rental_time_key,
                    1,  # count_returns
                    1,  # count_rentals
                    rental_duration,
                    rental["last_update"],
                    rental["rental_id"],
                ),
            )

        dw_conn.commit()
        print("fact_rental populated successfully")

    except Error as e:
        print(f"Error populating fact_rental: {e}")
    finally:
        cursor.close()
        dw_cursor.close()


def drill_down_analysis():
    """执行钻取分析"""
    conn = create_connection(dw_config)
    cursor = conn.cursor(dictionary=True)
    cursor.execute(
        """
        SELECT 
            d.year_month_abbreviation AS month,
            COUNT(*) AS rental_count
        FROM 
            fact_rental f
        JOIN 
            dim_date d ON f.rental_date_key = d.date_key
        GROUP BY 
            d.year_month_abbreviation
        ORDER BY 
            d.year_month_abbreviation;
        """
    )
    results = cursor.fetchall()
    print(results)

    # Convert to DataFrame for visualization
    df_monthly = pd.DataFrame(results, columns=["month", "rental_count"])

    # Visualization
    plt.figure(figsize=(12, 6))
    plt.bar(df_monthly["month"], df_monthly["rental_count"], color="skyblue")
    plt.title("Monthly Rental Trends")
    plt.xlabel("Month")
    plt.ylabel("Number of Rentals")
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.show()

    cursor.close()
    conn.close()


def slice_analysis(category="Action"):
    """执行切片分析"""
    conn = create_connection(dw_config)
    cursor = conn.cursor(dictionary=True)
    cursor.execute(
        f"""
        SELECT 
            CONCAT(dd.month_name, ' ', dd.year4) AS month_year,
            COUNT(*) AS rental_count
        FROM 
            fact_rental fr
        JOIN 
            dim_film df ON fr.film_key = df.film_key
        JOIN 
            dim_date dd ON fr.rental_date_key = dd.date_key
        WHERE 
            df.film_in_category_{category.lower()} = 'Yes'
        GROUP BY 
            dd.year4, dd.month_name, dd.month_number
        ORDER BY 
            dd.year4, dd.month_number;
        """
    )
    results = cursor.fetchall()
    print(results)

    columns = [desc[0] for desc in cursor.description]
    df = pd.DataFrame(results, columns=columns)

    cursor.close()
    conn.close()

    if df is None or df.empty:
        print("没有数据可绘制")
        return

    plt.figure(figsize=(12, 6))
    plt.bar(df["month_year"], df["rental_count"], color="#4CAF50")

    # 设置图表样式
    plt.xlabel("Month", fontsize=12)
    plt.ylabel("Rental Cnt", fontsize=12)
    plt.title(f"{category} Rental Trends", fontsize=14)
    plt.xticks(rotation=45, ha="right")
    plt.grid(axis="y", linestyle="--", alpha=0.7)

    # 自动调整布局
    plt.tight_layout()
    plt.show()


def main():
    # 创建数据库连接
    conn = create_connection(config)
    dw_conn = create_connection(dw_config)

    if conn and dw_conn:
        try:
            populate_dim_date(dw_conn)
            populate_dim_time(dw_conn)
            populate_dim_staff(conn, dw_conn)
            populate_dim_customer(conn, dw_conn)
            populate_dim_store(conn, dw_conn)
            populate_dim_actor(conn, dw_conn)
            populate_dim_film(conn, dw_conn)
            populate_film_actor_bridge(conn, dw_conn)
            populate_fact_rental(conn, dw_conn)

            print("Data warehouse population completed successfully!")

            # 执行钻取分析
            drill_down_analysis()
            slice_analysis(category="Action")

            print("Analysis completed successfully!")

        except Error as e:
            print(f"Error during ETL process: {e}")
        finally:
            conn.close()
            dw_conn.close()
    else:
        print("Failed to connect to databases")


if __name__ == "__main__":
    main()

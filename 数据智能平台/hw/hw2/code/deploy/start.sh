#!/bin/bash

docker compose -f compose.yml build

docker compose -f compose.yml up -d

spark-submit --master local[*] \
             --deploy-mode client \
             --conf spark.sql.extensions=io.delta.sql.DeltaSparkSessionExtension \
             --conf spark.sql.catalog.spark_catalog=org.apache.spark.sql.delta.catalog.DeltaCatalog \
             --packages io.delta:delta-core_2.13:2.4.0 --repositories https://repo1.maven.org/maven2 \
             /root/code/src/data_receiver.py 
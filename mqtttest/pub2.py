import pika
import os
import time
import random


0


properties = pika.BasicProperties(
        content_type='application/json',
        content_encoding='utf-8',
        delivery_mode=2,
    )
# 1. 创建一个到RabbitMQ server的连接，如果连接的不是本机，
# 则在pika.ConnectionParameters中传入具体的ip和port即可
connection = pika.BlockingConnection(
    pika.ConnectionParameters(
            "140.125.207.230",
        ))
# 2. 创建一个channel
channel = connection.channel()
# 3. 创建队列，queue_declare可以使用任意次数，
# 如果指定的queue不存在，则会创建一个queue，如果已经存在，
# 则不会做其他动作，官方推荐，每次使用时都可以加上这句
channel.queue_declare(queue='hello')
while True:
    msg ="測試訊息編號:"+str(random.randint(1,50))
    # 4. 发布消息
    channel.basic_publish(
        exchange='',  # RabbitMQ中所有的消息都要先通过交换机，空字符串表示使用默认的交换机
        routing_key='hello',  # 指定消息要发送到哪个queue
        body=msg,
        properties=properties
        )  # 消息的内容
    print("Send Message",msg)
    time.sleep(5)
# 5. 关闭连接
connection.close()
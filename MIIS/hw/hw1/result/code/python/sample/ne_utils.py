import base64
import hashlib
import hmac
import os
import shutil
from datetime import datetime
from time import mktime
from urllib.parse import urlencode
from wsgiref.handlers import format_date_time

from sample.exception import FileNotFoundException, AssembleHeaderException
from urllib import parse


def get_file_bytes(fd):
    """
    根据文件路径（一般相对）获取二进制数据
    :param fd: 文件路径
    :return: 二进制数据，不存在时为 None供后续判断
    """
    if os.path.exists(fd):
        with open(fd, "rb") as f:
            wav_maker = f.read(4)
            if b'RIFF' == wav_maker:
                f.seek(44, 0)
            else:
                f.seek(0, 0)
            f_data = f.read()
        f.close()
    else:
        raise FileNotFoundException("{}：文件不存在".format(fd))
    return f_data


def del_file(filepath):
    """
    删除某一目录下的所有文件或文件夹
    :param filepath: 路径
    :return:
    """
    del_list = os.listdir(filepath)
    for f in del_list:
        file_path = os.path.join(filepath, f)
        if os.path.isfile(file_path):
            os.remove(file_path)
        elif os.path.isdir(file_path):
            shutil.rmtree(file_path)


# 生成鉴权的url
def build_auth_request_url(request_url, method="POST", api_key="", api_secret=""):
    url_result = parse.urlparse(request_url)
    date = format_date_time(mktime(datetime.now().timetuple()))
    #print(date)
    signature_origin = "host: {}\ndate: {}\n{} {} HTTP/1.1".format(url_result.hostname, date, method, url_result.path)
    signature_sha = hmac.new(api_secret.encode('utf-8'), signature_origin.encode('utf-8'),
                             digestmod=hashlib.sha256).digest()
    signature_sha = base64.b64encode(signature_sha).decode(encoding='utf-8')
    #print(signature_sha)
    authorization_origin = "api_key=\"%s\", algorithm=\"%s\", headers=\"%s\", signature=\"%s\"" % (
        api_key, "hmac-sha256", "host date request-line", signature_sha)
    authorization = base64.b64encode(authorization_origin.encode('utf-8')).decode(encoding='utf-8')
    #print(authorization)
    values = {
        "host": url_result.hostname,
        "date": date,
        "authorization": authorization
    }
    #print("请求url:",request_url + "?" + urlencode(values))
    return request_url + "?" + urlencode(values)

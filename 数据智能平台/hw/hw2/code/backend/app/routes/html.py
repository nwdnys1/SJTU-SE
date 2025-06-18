from flask import Blueprint, send_from_directory, jsonify, abort
import os

# 创建蓝图
bp = Blueprint('html', __name__)

PCA_FOLDERS = [
    '/root/results/PCA/'
]

# 定义服务器上的文件夹路径
REPORT_FOLDERS = [
    '/root/results/Report/三联供',
    '/root/results/Report/冷机',
    '/root/results/Report/热机'
]

PREDICT_FOLDERS = [
    '/root/results/Predict',
]

FILE_FOLDERS = PCA_FOLDERS + REPORT_FOLDERS + PREDICT_FOLDERS

# 提供 HTML 文件服务
@bp.route('/files/<path:filename>', methods=['GET'])
def serve_file(filename):
    """
    Serve files from the specified report folders.
    """
    for folder in FILE_FOLDERS:
        file_path = os.path.join(folder, filename)
        if os.path.exists(file_path):
            return send_from_directory(folder, filename)
    # 如果文件不存在，返回 404
    abort(404, description=f"File {filename} not found")

# 获取所有 pca 文件的 API
@bp.route('/all_pca_files', methods=['GET'])
def list_files():
    """
    List all HTML files in the report folders.
    """
    html_files = []
    for folder in PCA_FOLDERS:
        if os.path.exists(folder):
            for file in os.listdir(folder):
                if file.endswith('.html'):
                    # 构造文件的完整路径
                    file_path = os.path.join(folder, file)
                    # 将文件名和相对路径返回
                    html_files.append({
                        'name': file,
                        'url': f'/files/{file}'
                    })
        else:
            print(f"Folder {folder} does not exist.")
    return jsonify(html_files)

# 获取所有报表文件的 API
@bp.route('/all_report_files', methods=['GET'])
def list_report_files():
    """
    List all report files in the report folders.
    """
    report_files = []
    for folder in REPORT_FOLDERS:
        if os.path.exists(folder):
            for file in os.listdir(folder):
                if file.endswith('.html'):
                    # 构造文件的完整路径
                    file_path = os.path.join(folder, file)
                    # 将文件名和相对路径返回
                    report_files.append({
                        'name': file,
                        'url': f'/files/{file}'
                    })
        else:
            print(f"Folder {folder} does not exist.")
    return jsonify(report_files)

# 获取所有预测文件的 API
@bp.route('/all_predict_files', methods=['GET'])
def list_predict_files():
    """
    List all prediction files in the report folders.
    """
    predict_files = []
    for folder in PREDICT_FOLDERS:
        if os.path.exists(folder):
            for file in os.listdir(folder):
                if file.endswith('.html'):
                    # 构造文件的完整路径
                    file_path = os.path.join(folder, file)
                    # 将文件名和相对路径返回
                    predict_files.append({
                        'name': file,
                        'url': f'/files/{file}'
                    })
        else:
            print(f"Folder {folder} does not exist.")
    return jsonify(predict_files)
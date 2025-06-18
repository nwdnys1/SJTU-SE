import os
from collections import defaultdict
import pandas as pd
from datetime import datetime

device_types = [
    "冷机",
    "冷水回水总管",
    "冷水供水总管",
    "热水供水总管",
    "热水回水总管",
    "供冷总管",
    "锅炉",
    "燃烧机",
    "发电机",
    "三联供",
    "发电机组"
]
device_dict = defaultdict(list)
all_data = defaultdict(dict)

def parse_xml_to_dict(xml_file):
    """
    解析XML文件，生成设备字典
    格式: {设备名称: [{filename: "xxx.txt", field: "描述"}, ...]}
    """
    
    
    try:

        
        # 假设XML格式如示例所示，每段包含设备名称和描述
        # 由于示例不是标准XML，这里做简化处理
        with open(xml_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # 分割每条记录
        records = content.split('.\n')[:-1]  # 去掉最后的空记录
        
        for record in records:
            # 提取设备名称和描述
            lines = record.strip().split('\n')
            if len(lines) < 2:
                continue
                
            # 第一行提取文件名
            file_line = lines[0].strip()
            filename = file_line.split()[0].strip('<>')
            
            # 第二行提取设备名和字段名
            desc_line = lines[1].strip()
            if 'rdfs:label' in desc_line:
                # 提取引号内的描述
                start = desc_line.find('"') + 1
                end = desc_line.rfind('"')
                description = desc_line[start:end]
                
                # 检查设备类型
                for device_type in device_types:
                    if device_type in description:
                        # 设备名称
                        pos = description.find(device_type)
                        device_name = description[:pos+len(device_type)].strip()
                        field= description[pos+len(device_type):].strip()
                        break
                
                # 添加到字典
                device_dict[device_name].append({
                    'filename': f"{filename}.txt",
                    'field': field
                })
                
    except Exception as e:
        print(f"Error parsing XML file: {e}")
    
    return dict(device_dict)

def read_txt_file(filepath):
    """
    读取txt文件，返回时间戳和数据的列表
    每行格式: 时间戳\t数据
    """
    data = []
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if line:
                    parts = line.split('\t')
                    if len(parts) == 2:
                        timestamp, value = parts
                        data.append((timestamp, float(value)))
    except FileNotFoundError:
        print(f"File not found: {filepath}")
    except Exception as e:
        print(f"Error reading file {filepath}: {e}")
    return data

def process_data_files(device_dict, data_folder):
    """
    处理所有数据文件，合并相同时间戳的数据
    返回: {设备名称: {时间戳: {字段1: 值1, 字段2: 值2, ...}}}
    """
    
    
    for device_name, fields in device_dict.items():
        # 为每个设备创建数据结构
        device_data = defaultdict(dict)
        
        for field_info in fields:
            filename = field_info['filename']
            field_desc = field_info['field']
            filepath = os.path.join(data_folder, filename)
            
            if not os.path.exists(filepath):
                continue
                
            # 读取txt文件数据
            records = read_txt_file(filepath)
            
            # 将数据按时间戳合并
            for timestamp, value in records:
                device_data[timestamp][field_desc] = value
                
        # 将设备数据添加到总数据中
        all_data[device_name] = dict(device_data)
    
    return dict(all_data)



def sort_and_impute_data(processed_data):
    """
    对每个设备的数据按时间戳排序，并进行缺失值插补
    """
    for device_name, time_data in processed_data.items():
        if not time_data:
            continue
            
        # 1. 按时间戳排序
        # 将时间字符串转换为datetime对象以便正确排序
        sorted_timestamps = sorted(time_data.keys(), key=lambda x: datetime.strptime(x, "%Y-%m-%dT%H:%M:%SZ"))
        
        # 获取所有可能的字段名
        all_fields = set()
        for fields in time_data.values():
            all_fields.update(fields.keys())
        all_fields = list(all_fields)
        
        # 2. 检查并插补缺失值
        for i, timestamp in enumerate(sorted_timestamps):
            current_data = time_data[timestamp]
            
            # 检查每个字段是否存在
            for field in all_fields:
                if field not in current_data:
                    # 寻找前一个非缺失值
                    prev_val = None
                    for j in range(i-1, -1, -1):
                        prev_timestamp = sorted_timestamps[j]
                        if field in time_data[prev_timestamp]:
                            prev_val = time_data[prev_timestamp][field]
                            break
                    
                    # 寻找后一个非缺失值
                    next_val = None
                    for j in range(i+1, len(sorted_timestamps)):
                        next_timestamp = sorted_timestamps[j]
                        if field in time_data[next_timestamp]:
                            next_val = time_data[next_timestamp][field]
                            break
                    
                    # 插值逻辑
                    if prev_val is not None and next_val is not None:
                        # 前后都有值，取平均
                        imputed_val = (prev_val + next_val) / 2
                    elif prev_val is not None:
                        # 只有前值
                        imputed_val = prev_val
                    elif next_val is not None:
                        # 只有后值
                        imputed_val = next_val
                    else:
                        # 理论上不应该出现这种情况
                        raise ValueError(f"No valid neighbors found for imputation at {timestamp}, field {field}")
                    
                    # 填充缺失值
                    time_data[timestamp][field] = imputed_val
        
        # 重新构建按时间排序的数据字典
        sorted_data = {}
        for timestamp in sorted_timestamps:
            sorted_data[timestamp] = time_data[timestamp]
        
        processed_data[device_name] = sorted_data
    
    return processed_data

def save_to_csv(data_dict, output_folder):
    """
    将每个设备的数据保存为单独的CSV文件
    文件名格式: {设备名称}.csv
    """
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)
    
    for device_name, time_data in data_dict.items():
        # 转换为DataFrame
        rows = []
        for timestamp, fields in time_data.items():
            row = {'timestamp': timestamp}
            row.update(fields)
            rows.append(row)
        
        if rows:
            df = pd.DataFrame(rows)
            # 保存为CSV，文件名是设备名称
            csv_file = os.path.join(output_folder, f"{device_name}.csv")
            df.to_csv(csv_file, index=False, encoding='utf-8-sig')
            print(f"Saved {device_name} data to {csv_file}")

def main():
    # 配置路径
    data_folder = './data'
    xml_file = os.path.join(data_folder, 'pos.ttl')
    data_flie = './data/positions'
    output_folder = 'output'
    
    # 1. 解析XML文件
    print("Parsing XML file...")
    device_dict = parse_xml_to_dict(xml_file)
    print(f"Found {len(device_dict)} devices")
    
    # 2. 处理数据文件
    print("Processing data files...")
    processed_data = process_data_files(device_dict, data_flie)
    
    # 新增步骤：排序和插补缺失值
    print("Sorting and imputing missing values...")
    processed_data = sort_and_impute_data(processed_data)
        
    # 3. 保存为CSV
    print("Saving to CSV files...")
    save_to_csv(processed_data, output_folder)
    print(f"All data saved to {output_folder}")

if __name__ == "__main__":
    main()
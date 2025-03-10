import requests
import time

# 获取多长时间的数据 单位小时
hours = 6
# 设备运行时长 单位秒
air_time = 0
fan_time = 0
light_time = 0
# 模拟功耗 空调800W，排气扇40W，灯光40W
air_power = 0.8
fan_power = 0.04
light_power = 0.04
# 总的能耗 单位 kWh
W_real = 0
W_before = ( air_power + fan_power + light_power ) * hours

# 设备属性记录查询API 需要设备在线
API_URL = "https://iot-api.heclouds.com/thingmodel/query-device-property-history"

# 设备信息
product_id = "OcFyEAzH58"
device_names = ["office_1", "office_2"]
tokens = [
    "version=2018-10-31&res=products%2FOcFyEAzH58%2Fdevices%2Foffice_1&et=1761720044&method=md5&sign=JFXruNyGm%2BFixVErw7Mmdg%3D%3D",    # office_1
    "version=2018-10-31&res=products%2FOcFyEAzH58%2Fdevices%2Foffice_2&et=1762233160&method=md5&sign=mCWMJY%2BFSHLZzcBlstleCw%3D%3D",    # office_2
]
identifiers = [
    "airconductor", "fan", "light"
]

# 循环遍历设备和标识符
for i, device_name in enumerate(device_names):
    token = tokens[i]
    current_timestamp = int(time.time()) * 1000  # 获取当前时间的毫秒级时间戳

    # 请求headers
    headers = {
        "authorization": token,
        "Content-Type": "application/json"
    }

    # 循环遍历标识符
    for j, identifier in enumerate(identifiers):
        # 请求params
        params = {
            "product_id": product_id,
            "device_name": device_name,
            "identifier": identifier,
            "start_time": current_timestamp - hours * 60 * 60 * 1000,  # 获取hours小时前的历史记录
            "end_time": current_timestamp,
            "limit": 1080  # 设置每次请求的数据量限制
        }

        # 发送GET请求
        try:
            response = requests.get(API_URL, headers=headers, params=params)
            response.raise_for_status()  # 如果响应状态码不是200，则引发HTTPError异常
            # 请求成功，解析返回的数据
            print(f"Response Data for {device_name} ({identifier}):", response.json())
            # 获取data中的数据
            data_list = response.json()['data']['list']
            print(f"{device_name} {identifier} data.list:", data_list)
            for item in data_list:
                value = item['value']
                time = item['time']
                if value == 'true':
                    if identifier == 'airconductor':
                        # print(f"{identifier}")
                        air_time += 1
                    elif identifier == 'fan':
                        # print(f"{identifier}")
                        fan_time += 1
                    elif identifier == 'light':
                        # print(f"{identifier}")
                        light_time += 1

        except requests.exceptions.HTTPError as http_err:
            print(f"HTTP error occurred for {device_name} ({identifier}): {http_err}")
        except requests.exceptions.ConnectionError as conn_err:
            print(f"Error connecting to API for {device_name} ({identifier}): {conn_err}")
        except requests.exceptions.Timeout as timeout_err:
            print(f"The request timed out for {device_name} ({identifier}): {timeout_err}")
        except requests.exceptions.RequestException as req_err:
            print(f"An error occurred for {device_name} ({identifier}): {req_err}")

# 运行时间换算为小时
air_time = air_time / 60 / 60
fan_time = fan_time / 60 / 60
light_time = light_time / 60 / 60

W_real = air_power * air_time + fan_power * fan_time + light_power * light_time
W_reduction = W_before - W_real
CO2_reduction = W_reduction * 0.997  # 单位 kg
reduction_percentage = W_reduction / W_before * 100 # 单位百分率
print(f"当前至 {hours:.2f} 小时前 设备运行情况如下：")
print(f"空调: {air_time:.2f} h")
print(f"排气: {fan_time:.2f} h")
print(f"灯光: {light_time:.2f} h")
print(f"总能耗: {W_real:.2f} kWh 减少能耗: {W_reduction:.2f} kWh")
print(f"减排量: {CO2_reduction:.2f} kg 减排率: {reduction_percentage:.2f}%")




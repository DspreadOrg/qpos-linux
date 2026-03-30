#!/bin/bash

# 检查是否传递了正确数量的参数
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <zip_file>"
    exit 1
fi

# 获取传递的参数
zip_file=$1

# 检查文件是否存在
if [ ! -f "$zip_file" ]; then
    echo "File not found!"
    exit 1
fi

# 计算 zip 文件的 MD5 值，并将其存储在变量 md5_value 中
md5_value=$(md5sum "$zip_file" | awk '{ print $1 }')

# 获取文件名和扩展名（包括扩展名在内的完整文件名）
filename=$(basename "$zip_file")

echo "文件名: $filename"
echo "MD5 值: $md5_value"

# 更新 JSON 文件中的 MD5 值和文件名
# 使用 sed 将这些值写入到 resource.json 文件中
sed -i "s/\"fileName\": *\"[^\"]*\"/\"fileName\": \"$filename\"/" resource.json
sed -i "s/\"md5\": *\"[^\"]*\"/\"md5\": \"$md5_value\"/" resource.json

# 可选：如果不需要备份文件，可以删除 .bak 文件
rm -f resource.json.bak

echo "MD5 hash and file information updated successfully in resource.json"

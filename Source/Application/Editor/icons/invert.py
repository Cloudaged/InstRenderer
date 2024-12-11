import cv2
import os
import numpy as np

def convert_images_to_color(input_folder, output_folder, target_color):
    # 确保输出文件夹存在
    os.makedirs(output_folder, exist_ok=True)
    
    # 遍历输入文件夹中的所有文件
    for filename in os.listdir(input_folder):
        if filename.lower().endswith('.png'):  # 仅处理 PNG 文件
            input_path = os.path.join(input_folder, filename)
            output_path = os.path.join(output_folder, filename)
            
            try:
                # 读取图像，包含 Alpha 通道
                image = cv2.imread(input_path, cv2.IMREAD_UNCHANGED)
                if image is None:
                    print(f"无法读取图像: {filename}")
                    continue

                # 检查是否包含 Alpha 通道
                if image.shape[2] == 4:  # RGBA 图像
                    # 分离通道
                    _, _, _, a = cv2.split(image)
                    # 创建目标颜色 (B, G, R) 数组
                    target_rgb = np.full((image.shape[0], image.shape[1], 3), target_color, dtype=np.uint8)
                    # 合并目标 RGB 和原来的 Alpha 通道
                    converted_image = cv2.merge([target_rgb[..., 0], target_rgb[..., 1], target_rgb[..., 2], a])
                else:
                    print(f"图像没有 Alpha 通道: {filename}")
                    # 对没有透明度的图像，直接生成固定颜色
                    target_rgb = np.full(image.shape, target_color, dtype=np.uint8)
                    converted_image = target_rgb

                # 保存到输出文件夹
                cv2.imwrite(output_path, converted_image)
                print(f"颜色转换完成: {filename}")
            except Exception as e:
                print(f"处理 {filename} 时出错: {e}")

# 输入文件夹和输出文件夹路径
input_folder = "D:\code_lib\AGProject\InstRenderer\Source\Application\Editor\icons\src"
output_folder = "D:\code_lib\AGProject\InstRenderer\Source\Application\Editor\icons\grey"

targetColor = (203,203,203)

convert_images_to_color(input_folder, output_folder,targetColor)
import cv2
import os
import numpy as np

def invert_images_with_alpha(input_folder, output_folder):
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
                    b, g, r, a = cv2.split(image)
                    # 对 RGB 通道执行反色
                    inverted_rgb = cv2.merge([255 - b, 255 - g, 255 - r])
                    # 合并反色后的 RGB 和原来的 Alpha 通道
                    inverted_image = cv2.merge([255 - b, 255 - g, 255 - r, a])
                else:
                    print(f"图像没有 Alpha 通道: {filename}")
                    # 对没有透明度的图像直接执行反色
                    inverted_image = 255 - image

                # 保存到输出文件夹
                cv2.imwrite(output_path, inverted_image)
                print(f"反色完成: {filename}")
            except Exception as e:
                print(f"处理 {filename} 时出错: {e}")

# 输入文件夹和输出文件夹路径
input_folder = "D:\code_lib\AGProject\InstRenderer\Source\Application\Editor\icons"
output_folder = "D:\code_lib\AGProject\InstRenderer\Source\Application\Editor\icons\invert"

invert_images_with_alpha(input_folder, output_folder)
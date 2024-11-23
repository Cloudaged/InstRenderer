import subprocess
import os 
import argparse 

def GetFileWithExtension(path,extension):
    files = []
    for fileName in os.listdir(path):
        if os.path.splitext(fileName)[1]==extension:
            files.append(fileName)
    return files

parser = argparse.ArgumentParser()

# 添加命令行参数
parser.add_argument('-path', type=str, help='Path of project', required=True)
args = parser.parse_args()


srcPath = args.path+"/Asset/Shader/slang"
extension = ".slang"

slangcPath = args.path+'/tools/slang/slangc.exe'
outputPath = args.path+'/Asset/Shader/slangSPV'

target = ['-target','spirv']
entry = ['-entry','main']

slangFiles =GetFileWithExtension(srcPath,extension)
for file in slangFiles:
    command = [slangcPath]+entry+target+[srcPath+'/'+file,'-o',outputPath+'/'+file+'.spv']
    process= subprocess.Popen(command,stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()  # 获取输出和错误信息
    returncode = process.returncode  # 获取退出码

    if returncode !=0 :
        print(f"Failed compile slang shader: {file}. Message: {stderr.decode()}\n")
    else :
        print(f"compile successed: {file}\n")


'''
# 构建命令
command = ["slangc","-target","glsl","-stage",args.stage,args.name,"-o","example.glsl"]


# 执行命令
print(f"Running command: {' '.join(command)}")
subprocess.run(command)
'''
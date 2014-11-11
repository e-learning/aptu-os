__author__ = 'muratov'
import generation
import os
if __name__ == '__main__':
    generation.list_N = [10000]
    generation.file_task1 = "testfile1"
    generation.max_thread = 10
    generation.task1()

    l = open("testfile1", "r").readlines()

    thread = 0
    max = -1
    for i, line in list(enumerate(l)):
        if int(line.split(' ')[2]) > max:
            max = int(line.split(' ')[2])
            thread = i
    print("good tier is {0} threads".format(thread))
    os.system("rm testfile1")
'''
/*
 * This file is part of the [Apollo] project.
 * Copyright [2023] [Rui Zhang] and other contributors.
 * See the LICENSE file in the project root directory for the full license information.
 * 
 * The sharing rights for this project belong to all those who have contributed to the project.
 * 
 * Thanks to the following contributors for their dedication and support of this project:
 *   - Rui Zhang 
*/
'''

def replace (file, old, new):
    code = rfile(file)
    code = code.replace(old, new)
    rwfile(file, code)

def rfile (file):
    with open(file, encoding = 'UTF-8') as f:
        read_all = f.read()
        f.close()
    return read_all

def rwfile (file, data):
    with open(file, 'w', encoding = 'UTF-8') as f:
        f.write(data)
        f.close()


import os

'''path'''
envpath = os.getenv('APOLLO_HOME')
listpath = envpath + 'separation/func/s_list'
modepath = envpath + 'separation/func/s_mode'
sfilepath = envpath + 'separation/func/s_file'
'''get'''
slist = rfile(listpath)
smode = rfile(modepath)
sfile = rfile(sfilepath)

index = int(slist)

for i in range(index):
    op   = envpath + 'separation/func/' + str(i + 1) + '.o'
    np   = envpath + 'separation/func/' + str(i + 1) + '.n'
    o    = rfile(op)

    olds = o
    news = ' CALL' + rfile(np) + ' '

    if smode == 's':
        replace(sfile, olds, news)
    elif smode == 'r':
        replace(sfile, news, olds)





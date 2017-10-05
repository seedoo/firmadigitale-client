# -*- coding: utf-8 -*-
# Utils for zip files

import os
import sys
import zipfile
import logging
from zipfile import ZipFile
from StringIO import StringIO


try:
    # py2exe fix
    CD = os.path.dirname(os.path.abspath(__file__))
    cf = ("%s\\config.ini" % (CD)).replace("library.zip\\", "")
    execfile(cf, globals(), locals())
except Exception, ex:
    logging.error("invalid config file! (webutils.py) %s" % ex)
    sys.exit(1)


def get_file_list(zip_path):
    """
    Read in memory a list of zip archive
    return list of files
    """
    result = []
    if os.path.isfile(zip_path):
        filezip = ZipFile(zip_path)
        for name in filezip.namelist():
            result.append({
                      'name': '%s' % (name)
                      }
            )
    return result


def get_token_from_zip(zip_path):
    """
    Read token from INFO file in a zip file
    return token/None
    """
    result = None
    if os.path.isfile(zip_path):
        filezip = ZipFile(zip_path)
        for name in filezip.namelist():
            if name == 'INFO' or name == 'info':
                filelines = filezip.open(name, 'r').readlines()
                for i, line in enumerate(filelines):
                    if i == 1:
                        result = line.replace(" ", "")
                        result = result.replace("\n", "")
                        result = result.replace("\r", "")
                        result = result.replace("\t", "")
    return result


def check_resetconfig_from_zip(zip_path):
    """
    Check for reset config on INFO file in a zip file
    return True/False
    """
    result = False
    if os.path.isfile(zip_path):
        filezip = ZipFile(zip_path)
        for name in filezip.namelist():
            if name == 'INFO' or name == 'info':
                filelines = filezip.open(name, 'r').readlines()
                for i, line in enumerate(filelines):
                    if i == 2:
                        v = line.replace(" ", "")
                        v = v.replace("\n", "")
                        v = v.replace("\r", "")
                        v = v.replace("\t", "")
                        if v.lower().find("reset") > -1:
                            result = True
    return result


def check_serverconfig_from_zip(zip_path):
    """
    Check for server-db config on INFO file in a zip file
    return Tuple (server, dbname)/False
    """
    result = False
    if os.path.isfile(zip_path):
        filezip = ZipFile(zip_path)
        for name in filezip.namelist():
            if name == 'INFO' or name == 'info':
                filelines = filezip.open(name, 'r').readlines()
                for i, line in enumerate(filelines):
                    if i == 0 and line.find(",") > -1:
                        v = line.replace(" ", "")
                        v = v.replace("\n", "")
                        v = v.replace("\r", "")
                        v = v.replace("\t", "")
                        values = v.split(",")
                        if len(values) == 2:
                            result = (values[0], values[1])
    return result


def extract_to_tmp(zip_path):
    """
    Extract content of zip file to tmp folder
    return list of files
    """
    result = []
    if os.path.isfile(zip_path):
        filezip = ZipFile(zip_path)
        current_tmp_folder = os.path.join(TEMP_FOLDER, DEFAULT_TOS_WORKING_PATH)
        filezip.extractall(current_tmp_folder)
        for name in filezip.namelist():
            current_file_path = os.path.join(current_tmp_folder, name)
            if os.path.isfile(current_file_path):
                result.append({
                          'name': '%s' % (name),
                          'path': '%s' % (current_file_path)
                          }
                )
    return result


def compose_zipfile(filelist, dest_filepath):
    """
    By a filelist with path and name compose
    a zip file.
    Remove previous file if exists
    Create folder of destination
    return None/StringFilePath
    """
    # REMOVE PREVIOUS FILE
    if os.path.isfile(dest_filepath):
        if not os.path.isdir(dest_filepath):
            os.unlink(dest_filepath)
    # CREATE FOLDERS
    d = os.path.dirname(dest_filepath)
    if not os.path.exists(d):
        os.makedirs(d)
    # COMPOSE ZIP
    memfile = StringIO()
    filezip = ZipFile(memfile, 'w', zipfile.ZIP_DEFLATED, False)
    result = None
    if len(filelist) > 0:
        for f in filelist:
            filepath = f['path']
            filename = f['name']
            if os.path.isfile(filepath):
                content = open(filepath, "rb").read()
                filezip.writestr(filename, content)
        filezip.close()
        memfile.seek(0)
        f = open(dest_filepath, 'wb')
        f.write(memfile.read())
        f.close()
        result = dest_filepath
    return result

# -*- coding: utf-8 -*-
# Utils for PSC CARD READER
# PSC_DATA was a Manager().dict for sharing between various processes

import subprocess
import time
import os
import sys
import logging
from datetime import datetime

try:
    # py2exe fix
    CD = os.path.dirname(os.path.abspath(__file__))
    cf = ("%s\\config.ini" % (CD)).replace("library.zip\\", "")
    execfile(cf, globals(), locals())
except Exception, ex:
    logging.error("invalid config file! (webutils.py) %s" % ex)
    sys.exit(1)


def _convert_month(date_str):
    """
    Convert string date, month name to month number
    """
    months = {
        'Jan': '01',
        'Feb': '02',
        'Mar': '03',
        'Apr': '04',
        'May': '05',
        'Jun': '06',
        'Jul': '07',
        'Aug': '08',
        'Sep': '09',
        'Oct': '10',
        'Nov': '11',
        'Dec': '12'
    }
    month = date_str[:3]
    return date_str.replace(month, months[month])


def check_psc():
    """
    Check presence of a smart card reader and card
    return True/False Tuple (psc, smart_card)
    """
    result_psc = False
    result_smart_card = False
    if os.path.isfile(CMDS['check_psc']):
        startupinfo = subprocess.STARTUPINFO()
        startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        p = subprocess.Popen(
            [CMDS['check_psc'], '2>&1'],
            bufsize=1,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            startupinfo=startupinfo,
        )
        stdout, stderr = p.communicate(input="\r\n")
        execution = stdout+stderr
        if execution.lower().find("no slot with a token") > -1:
            result_psc = True
        if execution.lower().find("using") > -1:
            result_psc = True
            result_smart_card = True
    else:
        if DEBUG:
            logging.error("cmd file not found! (check_psc)")
    return (result_psc, result_smart_card)


def create_certificate(PSC_DATA):
    """
    Create certificate.pem
    Result True/False
    """
    result_psc = PSC_DATA['psc']
    result_smart_card = PSC_DATA['smart_card']
    result = False
    PSC_DATA['signing'] = True  # make lock for other processes
    if result_psc and result_smart_card:
        if PSC_DATA['certificate_id'] != 0:
            if os.path.isfile(CMDS['create_certificate']):
                startupinfo = subprocess.STARTUPINFO()
                startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
                p = subprocess.Popen(
                    [CMDS['create_certificate'], PSC_DATA['certificate_id']],
                    bufsize=1,
                    stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    startupinfo=startupinfo,
                )
                p.communicate(input="\r\n")
                if os.path.isfile(DEFAULT_OPENSSL_PEM_PATH):
                    result = True
            else:
                if DEBUG:
                    logging.error("cmd file not found! (create_certificate)")
    PSC_DATA['signing'] = False
    return result


def get_certificate_id(PSC_DATA):
    """
    Get certificate id from smart card
    """
    result_psc = PSC_DATA['psc']
    result_smart_card = PSC_DATA['smart_card']
    PSC_DATA['signing'] = True  # make lock for other processes
    certificate_id = 0
    if result_psc and result_smart_card:
        if os.path.isfile(CMDS['get_certificate_id']):
            startupinfo = subprocess.STARTUPINFO()
            startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
            p = subprocess.Popen(
                [CMDS['get_certificate_id'], '2>&1'],
                bufsize=1,
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                startupinfo=startupinfo
            )
            stdout, stderr = p.communicate(input="\r\n")
            execution = stdout+stderr
            list_execution = execution.split("\n")
            try:
                certificate_id = \
                    list_execution[14].split(":")[1].replace(" ", "")
            except:
                try:
                    certificate_id = \
                        list_execution[2].split(":")[1].replace(" ", "")
                except:
                    certificate_id = 0
        else:
            if DEBUG:
                logging.error("cmd file not found! (get_certificate_id)")
    else:
        if DEBUG:
            logging.warning("no psc or smart card not inserted!")
    PSC_DATA['certificate_id'] = certificate_id
    PSC_DATA['signing'] = False
    return certificate_id


def get_certificate_info():
    """
    Get certificate info from pem file
    """
    result = {}
    if os.path.isfile(DEFAULT_OPENSSL_PEM_PATH):
        startupinfo = subprocess.STARTUPINFO()
        startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        p = subprocess.Popen(
            [CMDS['info_certificate'], '2>&1'],
            bufsize=1,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            startupinfo=startupinfo
        )
        stdout, stderr = p.communicate(input="\r\n")
        execution = stdout+stderr
        list_execution = execution.split("\n")
        for line in list_execution:
            if line.find("CN=") > -1:
                splitted_line = line.split(",")
                for sline in splitted_line:
                    if sline.find("CN=") > -1:
                        result['fullname'] = sline.split("/")[0]\
                            .replace("CN=", "").strip()
            elif line.lower().find("not before") > -1:
                dstart = line.split(": ")[1].replace("\n", "").strip()
                result['dstart'] = datetime.strptime(
                    _convert_month(dstart),
                    "%m %d %H:%M:%S %Y %Z"
                )
            elif line.lower().find("not after") > -1:
                dend = line.split(": ")[1].replace("\n", "").strip()
                result['dend'] = datetime.strptime(
                    _convert_month(dend),
                    "%m %d %H:%M:%S %Y %Z"
                )
    else:
        if DEBUG:
            logging.warning("certificate.pem not found! (info_certificate)")

    return result


def check_psc_process(PSC_DATA, timeout=DEFAULT_TIMEOUT_PSC):
    """
    To be used as a process.
    Check for psc and smart card ad set values on PSC_DATA Global Manager Dict
    Call create_certificate(PSC_DATA) for creating a certificate.pem/der
    """
    while True:
        if PSC_DATA.get('signing', False) == False:
            try:
                result_psc, result_smart_card = check_psc()
            except Exception, ex:
                if DEBUG:
                    logging.error("error psc process " % (ex))
                result_psc = False
                result_smart_card = False
            PSC_DATA['psc'] = result_psc
            PSC_DATA['smart_card'] = result_smart_card

            # IF NOT PSC OR SMARTCARD
            if not result_psc or not result_smart_card:
                PSC_DATA['certificate_id'] = 0
                PSC_DATA['user_fullname'] = None
                PSC_DATA['certificate_dstart'] = None
                PSC_DATA['certificate_dend'] = None

                # PURGE FILES
                try:
                    os.unlink(DEFAULT_OPENSSL_DER_PATH)
                except:
                    pass
                try:
                    os.unlink(DEFAULT_OPENSSL_PEM_PATH)
                except:
                    pass
            else:
                get_certificate_id(PSC_DATA)
                create_certificate(PSC_DATA)
        else:
            if DEBUG:
                logging.info("check psc stopped due to sign request!")
        time.sleep(timeout)


def check_certificate_process(PSC_DATA, timeout=DEFAULT_TIMEOUT_CERTIFICATE):
    """
    To be used as a process.
    Check for certificate ad set values on PSC_DATA Global Manager Dict
    """
    while True:
        certificate_info = get_certificate_info()
        if certificate_info.get("fullname"):
            dstart_str = datetime.strftime(
                certificate_info['dstart'],
                "%Y-%m-%d %H:%M:%S"
            )
            dend_str = datetime.strftime(
                certificate_info['dend'],
                "%Y-%m-%d %H:%M:%S"
            )
            PSC_DATA['user_fullname'] = certificate_info['fullname']
            PSC_DATA['certificate_dstart'] = dstart_str
            PSC_DATA['certificate_dend'] = dend_str
        else:
            PSC_DATA['certificate_id'] = 0
            PSC_DATA['user_fullname'] = None
        time.sleep(timeout)


def sign(PSC_DATA, source_file):
    """
    Sign a file
    output on same source_file path
    return Tuple(True/False|exception)
    """
    result = False
    ex = None
    result_psc = PSC_DATA['psc']
    result_smart_card = PSC_DATA['smart_card']
    PSC_DATA['signing'] = True  # make lock for other processes
    certificate_id = PSC_DATA['certificate_id']
    if result_psc and result_smart_card:
        if certificate_id != 0:
            if os.path.isfile(CMDS['sign']):
                startupinfo = subprocess.STARTUPINFO()
                startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
                p = subprocess.Popen(
                    [CMDS['sign'], '2>&1', certificate_id, source_file],
                    bufsize=1,
                    stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    startupinfo=startupinfo
                )
                stdout, stderr = p.communicate(input="\r\n")
                execution = stdout+stderr
                list_execution = execution.split("\n")
                result = True
                for line in list_execution:
                    if line.lower().find("locked") > -1:
                        if DEBUG:
                            logging.info(line)
                        result = False
                        ex = "pin locked"
                        break
                    if line.lower().find("incorrect") > -1:
                        if DEBUG:
                            logging.info(line)
                        result = False
                        ex = "pin incorrect"
                        break
            else:
                if DEBUG:
                    logging.error("cmd file not found! (sign)")
        else:
            if DEBUG:
                    logging.warning("certificate_id equals to 0 (sign)")
    else:
        if DEBUG:
            logging.warning("no psc or smart card not inserted!")
    PSC_DATA['signing'] = False
    return (result, ex)

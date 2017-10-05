# -*- coding: utf-8 -*-
# Utils for web requests

import os
import httplib
import urllib
import base64
import sys
import erppeek
import logging

try:
    # py2exe fix
    CD = os.path.dirname(os.path.abspath(__file__))
    cf = ("%s\\config.ini" % (CD)).replace("library.zip\\", "")
    execfile(cf, globals(), locals())
except Exception, ex:
    logging.error("invalid config file! (webutils.py) %s" % ex)
    sys.exit(1)


def send_file(server, url, filepath, token, dbname, SSL=False):
    """
    Send file to a specific url
    """
    result_upload = False
    result_ex = None
    if os.path.isfile(filepath):
        data = open(filepath, 'rb').read()
        encoded_data = base64.encodestring(data)
        headers = {
                   "Content-type": "application/x-www-form-urlencoded",
                   "Accept": "text/plain"
                   }
        params = urllib.urlencode({
            'filename': os.path.split(filepath)[1],
            'zfile': encoded_data,
            'token': token,
            'dbname': dbname
        })
        if SSL:
            conn = httplib.HTTPSConnection(server)
        else:
            conn = httplib.HTTPConnection(server)
        try:
            conn.request("POST", url, params, headers)
            response = conn.getresponse()
            html = response.read()
            if DEBUG:
                logging.info(html)
                logging.info(response.status)

            if response.status == 200:
                if html.lower().find("ok") > -1:
                    if DEBUG:
                        logging.info('upload success!')
                    result_upload = True
                    result_ex = None
                else:
                    result_upload = False
                    result_ex = html
            elif response.status == 404:
                result_upload = False
                result_ex = '404 not found'
            elif response.status == 504:
                result_upload = False
                result_ex = '504 timeout'
            elif response.status == 502:
                result_upload = False
                result_ex = '502 Bad gateway'
            elif response.status == 500:
                result_upload = False
                result_ex = '500 Internal server error'
            conn.close()
        except Exception, ex:
            result_upload = False
            result_ex = "Connection failed: %s" % (ex)
            if DEBUG:
                logging.error(result_ex)

    else:
        if DEBUG:
            logging.error("file to upload not found! (send_file)")
    return (result_upload, result_ex)


class OdooUser():
    """
    Odoo User Class
    """
    def __init__(self, odoo_server, odoo_db,
                 odoo_user, odoo_password, port=80):
        self.host = odoo_server
        self.port = port
        protocol = 'http://' if self.port != 443 else 'https://'
        self.db = odoo_db
        self.user = odoo_user
        self.password = odoo_password
        self.client = erppeek.Client(
                        "%s%s:%s" % (protocol, self.host, self.port),
                        self.db, self.user, self.password
                    )

    def update_pem(self, pem, dstart, dend):
        user_proxy = self.client.model('res.users')
        user_proxy.browse(self.client._execute.args[1])
        result = user_proxy.update_user_pem_line(pem, dstart, dend)
        return result

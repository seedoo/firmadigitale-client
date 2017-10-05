# -*- coding: utf-8 -*-

from distutils.core import setup
import py2exe

# A NULL EXE FRO PY2EXE BUILD BUG
setup(
      windows=[
               {
                "script": "null.py",
                "icon_resources": [(0, "icon_dsodoo.ico")]
                }
               ],
      console=['null.py']
)

setup(
      version="0.9.0.0",
      name="DsOdoo",
      description="Digital Signature for Odoo",
      data_files=[
                  'c:\\Python27\\Lib\\site-packages\\'
                  'gtk-2.0\\runtime\\bin\\librsvg-2-2.dll',
                  'c:\\Python27\\Lib\\site-packages\\'
                  'gtk-2.0\\runtime\\bin\\libcroco-0.6-3.dll',
                  'c:\\Python27\\Lib\\site-packages\\'
                  'gtk-2.0\\runtime\\bin\\libxml2-2.dll',
                  'icon_dsodoo.ico',
                  'cardok.png',
                  'cardko.png',
                  'header_logo.png',
                  'main.glade',
                  'config.glade',
                  'pin.glade',
                  'config.ini',
                ],
      # zipfile="libs/lib.zip",
      options={
               'py2exe': {
                          'skip_archive': False,
                          'bundle_files': 3,
                          'includes': [
                                       'cairo',
                                       'pango',
                                       'atk',
                                       'pangocairo',
                                       'gio',
                                       'erppeek',
                                       'gtk',
                                       'pygtk',
                                       ],
                          'excludes': []
                          }
               },
      windows=[
               {
                "script": "dsodoo.py",
                "version": "0.9.0.1",
                "company_name": u"Innoviù S.R.L.",
                "copyright": u"Copyright © 2014 Innoviù srl",
                "name": "DsOdoo",
                "description": "Digital Signature for Odoo",
                "icon_resources": [(0, "icon_dsodoo.ico")]
                }
               ],
      console=['dsodoo.py']
)

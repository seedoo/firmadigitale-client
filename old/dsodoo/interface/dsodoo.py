# -*- coding: utf-8 -*-

import sys
import os
import time
import subprocess
import gobject
import pango
import logging
import multiprocessing
from multiprocessing import Process, Manager
import about
import psc
import ziplib
import webutils
from datetime import datetime

try:
    import pygtk
    pygtk.require("2.0")
except:
    logging.error("pygtk maybe corrupted!")
    pass
try:
    import gtk
except Exception, ex:
    logging.error("Error importing gtk: %s" % (ex))
    sys.exit(1)

try:
    # py2exe fix
    global CD
    try:
        CD = os.path.dirname(os.path.abspath(__file__))
    except:
        CD = "%s" % (sys.executable.replace("dsodoo.exe", ""))
    cf = ("%s\\config.ini" % (CD))
    execfile(cf, globals(), locals())
except Exception, ex:
    logging.error("invalid config file! (dsodoo.py) %s" % ex)
    sys.exit(1)

global ODOO_SERVER
global ODOO_DB
global ODOO_SSL

if DEBUG:
    logging.basicConfig(
                        filename='%s\\dsodoo.log' % (CD),
                        filemode='w',
                        level=logging.DEBUG
                        )
else:
    # DISABLE PY2EXE LOG
    sys.stderr = sys.stdout

PROCESSES = []


class DsOdooGTK:
    """
    Firma Digitale Odoo GTK Interface
    """
    def __init__(self, zipfile):
        self.zipfile = zipfile

        # PURGE PREVIOUS FILES
        self.delete_certificates()
        self.delete_pin_from_config()
        self.delete_working_folder()
        # INIT MAIN INTERFACE
        filename = "%s\\main.glade" % (CD)
        builder = gtk.Builder()
        builder.add_from_file(filename)
        builder.connect_signals(self)
        self.window = builder.get_object("main_window")
        self.header = {}
        self.header['header_logo'] = builder.get_object("header_logo")
        self.header['fullname_label'] = builder.get_object("fullname")
        self.header['cardstatus_image'] = builder.get_object("cardstatus")
        self.header['header_logo'].set_from_file(
            "%s\\header_logo.png" % (CD)
        )
        self.body = builder.get_object('dialog-vbox1')
        self.window.menus = []
        self.window.menus.append(builder.get_object('menuitem_file'))
        self.window.menus.append(builder.get_object('menuitem_help'))
        self.btn_close = builder.get_object("btn_close")
        self.btn_sign = builder.get_object("btn_sign")
        self.btn_close.connect("clicked", self.close)
        self.btn_sign.connect("clicked", self.sign)
        self.window.connect("destroy", self.close)
        self.filedatastore = None
        self.filedatagrid = None

        # POPULATE FILE GRID DATA
        self.populate_filegrid_data()

        # CREATE MENU
        self.init_menu()

        # SHOW MAIN WINDOW
        self.window.show_all()
        self.btn_close.hide()

        # SHOW PIN
        # show pin for first time
        # self.show_pin_window(True)

        # ASYNC SELF REFRESH CALLBACKS
        self.update_header_info()
        gobject.timeout_add(1000, self.update_header_info)

        # CHECK FOR CONFIG
        self.check_config()

    def init_menu(self):
        """
        Init Menu
        """
        for menuroot in self.window.menus:
            for menuentry in menuroot.get_submenu():
                if menuentry.get_children()[0]\
                        .get_text().lower() == "esci":
                    menuentry.connect("activate", self.close)
                elif menuentry.get_children()[0]\
                        .get_text().lower() == "licenza":
                    menuentry.connect("activate", self.show_license)
                elif menuentry.get_children()[0]\
                        .get_text().lower() == "informazioni":
                    menuentry.connect("activate", self.show_info)

    def show_license(self, *args):
        """
        Show license
        """
        sw = gtk.ScrolledWindow()
        sw.set_size_request(700, 300)
        sw.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        textview = gtk.TextView()
        textview.set_justification(gtk.JUSTIFY_CENTER)
        textview.set_editable(False)
        textbuffer = textview.get_buffer()
        msg = about.fix_txt_gtk(about.LICENSE)
        textbuffer.set_text(msg)
        textview.show()
        sw.add(textview)
        message = gtk.MessageDialog(
            parent=self.window,
            type=gtk.MESSAGE_OTHER,
            buttons=gtk.BUTTONS_OK
        )
        message.set_position(gtk.WIN_POS_CENTER)
        message.get_children()[0].add(sw)
        sw.show()
        message.run()
        message.destroy()

    def show_info(self, *args):
        """
        Show Info
        """
        msg = "Version: %s\n" % (about.VERSION)
        msg += about.fix_txt_gtk(about.COPYRIGHT)
        message = gtk.MessageDialog(
            parent=self.window,
            type=gtk.MESSAGE_OTHER,
            buttons=gtk.BUTTONS_OK
        )
        message.set_position(gtk.WIN_POS_CENTER)
        message.set_markup(pango.parse_markup(msg)[1])
        message.run()
        message.destroy()

    def show_error(self, parent=None, msg=''):
        """
        Create a MessageDialog for displaying a error
        """
        message = gtk.MessageDialog(
            parent=parent,
            type=gtk.MESSAGE_OTHER,
            buttons=gtk.BUTTONS_CLOSE
        )
        message.set_position(gtk.WIN_POS_CENTER)
        message.set_markup(pango.parse_markup(msg)[1])
        message.run()
        message.destroy()

    def check_config(self):
        """
        Check config for initial setup
        """
        if ziplib.check_resetconfig_from_zip(self.zipfile):
            self.show_config_window()

    def update_header_info(self):
        """
        Update Header info
        """
        try:
            btn_sign_label = self.btn_sign.get_children()[0]
            cardstatus = self.header['cardstatus_image']
            if PSC_DATA['user_fullname']:
                self.header['fullname_label'].set_markup(
                        "Firmatario: %s" % (PSC_DATA['user_fullname'])
                                            )
            else:
                self.header['fullname_label'].set_markup("")
            if PSC_DATA['certificate_id'] != 0 and PSC_DATA['user_fullname']:
                cardstatus.set_from_file(
                    '%s\\cardok.png' % (CD)
                )
                cardstatus.set_tooltip_text("SmartCard OK!")
                btn_sign_label.set_sensitive(True)
            else:
                cardstatus.set_from_file(
                    '%s\\cardko.png' % (CD)
                )
                cardstatus.set_tooltip_text("SmartCard non trovata!")
                btn_sign_label.set_sensitive(False)
        except:
            pass
        return True

    def update_config_header_info(self):
        """
        Update Config Header info
        """
        try:
            btn_save_config_label = \
                self.config_window.btn_save_config.get_children()[0]
            cardstatus = self.config_window.header['cardstatus_image']
            if PSC_DATA['certificate_id'] != 0 and PSC_DATA['user_fullname']:
                cardstatus.set_from_file(
                    '%s\\cardok.png' % (CD)
                )
                cardstatus.set_tooltip_text("SmartCard OK!")
                btn_save_config_label.set_sensitive(True)
            else:
                cardstatus.set_from_file(
                    '%s\\cardko.png' % (CD)
                )
                cardstatus.set_tooltip_text("SmartCard non trovata!")
                btn_save_config_label.set_sensitive(False)
        except:
            pass
        return True

    def on_cell_toggled(self, widget, path):
        """
        Onclick on checkbox of filedatagrid (treeview)
        """
        self.filedatastore[path][2] = not self.filedatastore[path][2]
        widget.set_active(not widget.get_active())

    def update_filedatastore(self):
        """
        By Global PSC_DATA['files']
        update filedatastore (for treeview)
        By default set all files to sign
        """
        if PSC_DATA['files']:
            if len(PSC_DATA['files']) > 0:
                if not self.filedatastore:
                    self.filedatastore = gtk.ListStore(str, str, bool)
                    for f in PSC_DATA['files']:
                        if f['name'] != 'INFO' and f['name'] != ['info']:
                            self.filedatastore.append(
                                (f['name'], f['path'], True)
                            )

    def update_filedatagrid(self):
        """
        Update filedatagrid for displaying/selecting
        files to be signed
        """
        if not self.filedatagrid:
            self.filedatagrid = gtk.TreeView(self.filedatastore)
        self.filedatagrid.set_size_request(1024, 210)
        renderer_text = gtk.CellRendererText()
        column_text = gtk.TreeViewColumn(
                        "Nome", renderer_text, text=0
                    )
        column_text.set_min_width(500)
        column_text.set_max_width(500)
        self.filedatagrid.append_column(column_text)

        renderer_toggle = gtk.CellRendererToggle()
        renderer_toggle.connect("toggled", self.on_cell_toggled)
        column_toggle = gtk.TreeViewColumn(
                            "Seleziona", renderer_toggle, active=2
                        )
        column_toggle.set_alignment(0.5)
        self.filedatagrid.append_column(column_toggle)

        self.body.add(self.filedatagrid)

    def delete_pin_from_config(self):
        """
        Delete pin from openssl config
        """
        current_config = open(DEFAULT_OPENSSL_CONF_PATH, "r").readlines()
        new_config = ''
        for line in current_config:
            if line.lower().find("pin") == -1:
                new_config += line
        write_config = open(DEFAULT_OPENSSL_CONF_PATH, "w")
        write_config.write(new_config)
        write_config.close()

    def delete_certificates(self):
        """
        Delete certificates from folder
        """
        # PURGE FILES
        try:
            os.unlink(DEFAULT_OPENSSL_DER_PATH)
        except:
            pass
        try:
            os.unlink(DEFAULT_OPENSSL_PEM_PATH)
        except:
            pass

    def check_pin(self):
        """
        Check pin on config
        Return Pin/None
        """
        result = None
        current_config = open(DEFAULT_OPENSSL_CONF_PATH, "r").readlines()
        for line in current_config:
            if line.lower().find("pin") > -1:
                result = line.split("=")[1].replace(" ", "")
        return result

    def delete_working_folder(self):
        """
        Delete previous working folder files
        """
        # PURGE FILES
        current_tmp_folder = os.path.join(
                                TEMP_FOLDER,
                                DEFAULT_TOS_WORKING_PATH
                            )
        if os.path.isdir(current_tmp_folder):
            for root, dirs, files in os.walk(
                                        current_tmp_folder,
                                        topdown=False
                                    ):
                for name in files:
                    os.remove(os.path.join(root, name))
                for name in dirs:
                    os.rmdir(os.path.join(root, name))
            os.rmdir(current_tmp_folder)

    def write_pin_to_config(self, pin):
        """
        Write pin to openssl config
        """
        current_config = open(DEFAULT_OPENSSL_CONF_PATH, "r").readlines()
        new_config = ''
        for line in current_config:
            if line.lower().find("pin") == -1:
                new_config += line

        # check presence for line empty on end of file
        if new_config.split("\n")[-1] != "":
            new_config += "\n"
        new_config += "PIN = %s\n" % (pin)
        write_config = open(DEFAULT_OPENSSL_CONF_PATH, "w")
        write_config.write(new_config)
        write_config.close()

    def validate_pin(self, widget, pin_value_widget):
        """
        Validate inserted pin
        """
        pin = pin_value_widget.get_text()
        pin = pin.replace(" ", "")
        if len(pin) == 0:
            self.show_error(self.pin_window, "Pin non valido!")
            return False

        try:
            int(pin)
        except:
            self.show_error(self.pin_window, "Il Pin dev'essere numerico!")
            return False

        self.write_pin_to_config(int(pin))
        self.window.show()
        self.pin_window.hide()

        # IF NOT FIRST TIME SHOW PIN WINDOW DO SIGN
        if not self.pin_window.first_time_show:
            return self.sign()

    def validate_config(self, widget):
        """
        Validate config
        """
        if not widget.get_children()[0].is_sensitive():
            return False

        # VALIDATE DATA
        odoo_user = self.config_window.odoo_user_entry.get_text()
        odoo_password = self.config_window.odoo_password_entry.get_text()
        if odoo_user.replace(" ", "") == '':
            self.show_error(self.config_window, "Utente non valido!")
            return False
        if odoo_password.replace(" ", "") == '':
            self.show_error(self.config_window, "Password non valida!")
            return False

        # ODOO VALIDATE
        odoo_validation = True
        odoo_validation_ex = None
        odoo_port = 443 if ODOO_SSL else 80
        try:
            odoo_user = webutils.OdooUser(ODOO_SERVER, ODOO_DB,
                                          odoo_user, odoo_password, odoo_port)
            pem = open(DEFAULT_OPENSSL_PEM_PATH, "r").read()
            odoo_user.update_pem(
                pem,
                PSC_DATA['certificate_dstart'],
                PSC_DATA['certificate_dend']
            )
        except Exception, ex:
            odoo_validation = False
            odoo_validation_ex = about.fix_txt_gtk(str(ex))
            if DEBUG:
                logging.error(
                              "Error on connecting to odoo %s"
                              % (odoo_validation_ex)
                              )
        if not odoo_validation:
            self.show_error(self.config_window,
                            "Attenzione problema con il server Odoo!\n"
                            "%s" % (odoo_validation_ex))
            return False

        self.window.show()
        self.config_window.hide()

    def on_key_press_pin_value(self, widget, event):
        """
        on key press on pin value
        """
        # ENTER KEY
        if event.keyval == 65293:
            self.pin_window.btn_save_pin.emit("clicked")

    def show_pin_window(self, first_time_show=False):
        """
        show window for inserting pin
        """
        self.window.hide()
        filename = "%s\\pin.glade" % (CD)
        builder = gtk.Builder()
        builder.add_from_file(filename)
        builder.connect_signals(self)
        pin_value = builder.get_object("pin_value")
        pin_value.connect("key-press-event", self.on_key_press_pin_value)
        btn_close = builder.get_object("btn_close")
        btn_save_pin = builder.get_object("btn_save_pin")
        btn_close.connect("clicked", self.close_pin_window)
        btn_save_pin.connect("clicked", self.validate_pin, pin_value)
        self.pin_window = builder.get_object("pin_window")
        self.pin_window.connect("destroy", self.close_pin_window)
        self.pin_window.first_time_show = first_time_show
        self.pin_window.btn_save_pin = btn_save_pin
        self.pin_window.show_all()

    def show_config_window(self):
        """
        show window for config
        """
        self.window.hide()
        try:
            self.pin_window.hide()
        except:
            pass
        filename = "%s\\config.glade" % (CD)
        builder = gtk.Builder()
        builder.add_from_file(filename)
        builder.connect_signals(self)
        btn_close_config = builder.get_object("btn_close_config")
        btn_save_config = builder.get_object("btn_save_config")
        odoo_user_entry = builder.get_object("odoo_user")
        odoo_password_entry = builder.get_object("odoo_password")
        btn_close_config.connect("clicked", self.close)
        btn_save_config.connect("clicked", self.validate_config)
        self.config_window = builder.get_object("config_window")
        self.config_window.header = {}
        self.config_window.header['cardstatus_image'] = \
            builder.get_object("cardstatus")
        self.config_window.connect("destroy", self.close)
        self.config_window.btn_close_config = btn_save_config
        self.config_window.btn_save_config = btn_save_config
        self.config_window.odoo_user_entry = odoo_user_entry
        self.config_window.odoo_password_entry = odoo_password_entry
        self.config_window.show_all()

        # ASYNC SELF REFRESH CALLBACKS
        self.update_config_header_info()
        gobject.timeout_add(1000, self.update_config_header_info)

    def populate_filegrid_data(self):
        """
        Check extraction of file and populate
        filedatastore and update filedatagrid.
        On error close app
        """
        filelist = ziplib.extract_to_tmp(zipfile)
        if len(filelist) > 0:
            PSC_DATA['files'] = filelist
            self.update_filedatastore()
            self.update_filedatagrid()
        else:
            self.show_error(self.window,
                            "Spiacente, lista files da firmare vuota."
                            )
            sys.exit(1)

    def process_files_tos(self):
        """
        By self.filedatastore check for files to be signed.
        Process sign on tmp folder.
        return List
        """
        result = []
        PSC_DATA['signing'] = True
        if PSC_DATA['psc'] and PSC_DATA['smart_card']:
            if self.filedatastore:
                for tos_file in self.filedatastore:
                    if tos_file[2]:
                        if DEBUG:
                            logging.info("signing %s" % (tos_file[0]))
                        try:
                            sign_result, sign_ex = psc.sign(
                                                    PSC_DATA,
                                                    tos_file[1]
                                                )
                        except Exception, ex:
                            sign_result = False
                            sign_ex = ex
                        if DEBUG:
                            logging.info("sign result: %s | %s" % (
                                    sign_result,
                                    sign_ex
                            ))

                        if sign_ex == "pin incorrect":
                            self.show_error(self.window,
                                            "Pin errato! Attenzione dopo 3 "
                                            "tentativi la carta verra' "
                                            "bloccata.")
                            self.delete_pin_from_config()
                            break
                        if sign_ex == "pin locked":
                            self.show_error(self.window, "Pin bloccato!")
                            self.close()
                            break
                        result.append({
                            'name': "%s.p7m" % (tos_file[0]),
                            'signed': sign_result,
                            'path': "%s.p7m" % (tos_file[1])
                            if sign_result else None
                        })
            else:
                if DEBUG:
                    logging.error("no filedatastore (process_files_tos)")
        else:
            self.show_error(self.window,
                            "Nessun lettore e/o smart card presente!"
                            )
        PSC_DATA['signing'] = False
        return result

    def sign(self, *args):
        """
        Sign process
        """
        btn_sign_label = self.btn_sign.get_children()[0]
        if not btn_sign_label.is_sensitive():
            return False

        # CHECK FILE SELECTED
        c = 0
        if self.filedatastore:
            for tos_file in self.filedatastore:
                if tos_file[2]:
                    c += 1
        if c == 0:
            return False

        if DEBUG:
            logging.info("Server Odoo: %s | Db: %s" % (ODOO_SERVER, ODOO_DB))

        # CHECK PSC/SMARTCARD
        if not PSC_DATA['psc'] or not PSC_DATA['smart_card']:
            if DEBUG:
                logging.info("no psc or smartcard present (sign)")
            return False

        # CHECK FOR PIN
        pin = self.check_pin()
        if not pin:
            self.show_pin_window()
            return False

        # CHECK FOR CERTIFICATE_ID
        if PSC_DATA['certificate_id'] == 0:
            if DEBUG:
                logging.info("certificate_id equals to 0 (sign)")
            return False

        # START DIGITAL SIGN
        processed_files = self.process_files_tos()
        dest_filepath = os.path.join(
                            TEMP_FOLDER,
                            DEFAULT_TOS_WORKING_PATH,
                            "%s.zip" % (PSC_DATA['token']),
                        )
        if DEBUG:
            logging.info(processed_files)

        # BUILD LIST OF SUCCESS
        signed_files = []
        for f in processed_files:
            if f['signed'] == True:
                signed_files.append(f)

        if len(signed_files) == 0:
            return False
        try:
            zipfile = ziplib.compose_zipfile(signed_files, dest_filepath)
        except Exception, ex:
            if DEBUG:
                logging.error("error on compose zip file %s" % (ex))
            self.show_error(self.window,
                            "Spiacente, si e' verificato"
                            "un errore durante la firma."
                            )
            self.close()

        try:
            result_upload, result_ex = webutils.send_file(
                                        ODOO_SERVER,
                                        ODOO_URL,
                                        zipfile,
                                        PSC_DATA['token'],
                                        ODOO_DB)
        except Exception, ex:
            result_upload = False
            result_ex = ex
            if DEBUG:
                logging.error("fatal error on upload zip file %s" % (ex))
            self.show_error(self.window,
                            "Spiacente, si e' verificato "
                            "un errore durante il caricamento del file.\n"
                            "%s" % (result_ex)
                            )
            self.close()

        if result_upload:
            self.close()
        else:
            if DEBUG:
                logging.error("error on upload zip file %s" % (result_ex))
            self.show_error(self.window,
                            "Spiacente, si e' verificato "
                            "un errore durante il caricamento del file.\n"
                            "%s" % (result_ex)
                            )

    def close_pin_window(self, *args):
        """
        Close pin window
        """
        self.pin_window.hide()
        try:
            self.config_window.hide()
        except:
            pass
        self.window.show()

    def close_config_window(self, *args):
        """
        Close config window
        """
        try:
            self.pin_window.hide()
        except:
            pass
        self.config_window.hide()
        self.window.show()

    def close(self, *args):
        """
        Close app
        """
        self.delete_certificates()
        self.delete_pin_from_config()
        self.delete_working_folder()
        gtk.main_quit()

if __name__ == "__main__":
    multiprocessing.freeze_support()

    # CHECK ZIP FILE TO SIGN
    zipfile = None
    for arg in sys.argv:
        if arg.lower().find(DEFAULT_TOS_EXTENSION) > -1:
            if os.path.isfile(arg):
                zipfile = arg
                if DEBUG:
                    logging.info("file to sign: %s" % (zipfile))
            else:
                if DEBUG:
                    logging.error("file to sign not found: %s" % (arg))
    if not zipfile:
        message = gtk.MessageDialog(
            type=gtk.MESSAGE_OTHER,
            buttons=gtk.BUTTONS_CLOSE
        )
        message.set_position(gtk.WIN_POS_CENTER)
        message.set_markup(
            "Spiacente, file da firmare non accessibile."
        )
        message.run()
        message.destroy()
        sys.exit(1)

    try:
        token = ziplib.get_token_from_zip(zipfile)
        if DEBUG:
            logging.info("Token: %s" % (token))
    except Exception, ex:
        if DEBUG:
            logging.error("Error extracting token from info file: %s" % (ex))
        token = None

    try:
        tos_config = ziplib.check_serverconfig_from_zip(zipfile)
        if DEBUG:
            logging.info("Config INFO: %s" % (str(tos_config)))
    except Exception, ex:
        if DEBUG:
            logging.error("Error extracting config from info file: %s" % (ex))
        tos_config = None

    if not token:
        if DEBUG:
            logging.error("Token from info file was None!")
        message = gtk.MessageDialog(
            type=gtk.MESSAGE_OTHER,
            buttons=gtk.BUTTONS_CLOSE
        )
        message.set_position(gtk.WIN_POS_CENTER)
        message.set_markup(
            "Spiacente, file da firmare non valido."
        )
        message.run()
        message.destroy()
        sys.exit(1)

    if not tos_config:
        if DEBUG:
            logging.error("Config from info file was None!")
        message = gtk.MessageDialog(
            type=gtk.MESSAGE_OTHER,
            buttons=gtk.BUTTONS_CLOSE
        )
        message.set_position(gtk.WIN_POS_CENTER)
        message.set_markup(
            "Spiacente, file da firmare non valido."
        )
        message.run()
        message.destroy()
        sys.exit(1)

    # SET GLOBALS
    ODOO_SERVER = tos_config[0]
    ODOO_DB = tos_config[1]
    ODOO_SSL = False

    # START PSC/SMART_CARD CHECK PROCESS
    manager = Manager()
    PSC_DATA = manager.dict()
    PSC_DATA['psc'] = False
    PSC_DATA['smart_card'] = False
    PSC_DATA['signing'] = False
    PSC_DATA['certificate_id'] = 0
    PSC_DATA['user_fullname'] = None
    PSC_DATA['certificate_dstart'] = None
    PSC_DATA['certificate_dend'] = None
    PSC_DATA['token'] = token
    PSC_DATA['files'] = []
    psc_process = Process(
        target=psc.check_psc_process,
        args=(PSC_DATA,)
    )
    psc_process.start()
    PROCESSES.append(psc_process)
    # START CERTIFICATE PROCESS
    certificate_process = Process(
        target=psc.check_certificate_process,
        args=(PSC_DATA,)
    )
    certificate_process.start()
    PROCESSES.append(certificate_process)

    # START INTERFACE
    dsodoo = DsOdooGTK(zipfile)
    gtk.main()

    # KILL AL PROCESSESS
    for p in PROCESSES:
        p.terminate()

#!/usr/bin/python
# -*- coding: utf-8 -*-
# http://qixinbo.info/2017/12/04/pyqt4-widgets/
# gui.py

import sys
import serial
import serial.tools.list_ports
from PyQt4 import QtGui, QtCore
from PyQt4.QtCore import QThread
import numpy as np
import matplotlib.pyplot as plt 
import thread
import time

class CustomComboBox(QtGui.QComboBox):
    """docstring for CustomComboBox
    重载下拉列表，每次点击时搜索串口号，将其添加到列表中
    继承自QtGui.QComboBox
    参考自：
    https://blog.csdn.net/MaggieTian77/article/details/79205192
    """
    popupAboutToBeShown = QtCore.pyqtSignal()
    def __init__(self, parent=None):
        super(CustomComboBox, self).__init__(parent)
    def showPopup(self):
        '''
        点击弹出列表时执行以下操作：
        清空下拉列表内容
        搜索串口号
        将串口号添加到下拉列表
        如果没有串口，弹框提示“找不到”
        '''
        self.clear()
        SerialPort_list = serial.tools.list_ports.comports()
        if not SerialPort_list:
            msg = QtGui.QMessageBox().question(self, 'msg', "Can't find any Serial port", QtGui.QMessageBox.Ok)
        else:
            for port in SerialPort_list:
                self.addItem(port.device)
                #print(p.description) #返回设备名字
                #print(p.location) #返回设备在计算机上的位置
        QtGui.QComboBox.showPopup(self)#执行QtGui.QComboBox定义的操作

class MyThread(QThread):
    '''
    重载线程类，继承自QThread
    pyqt线程间通信,自定义信号，发出该信号时触发相应的槽，执行对应函数
    参考自：
    http://www.voidcn.com/article/p-risthuvv-bdr.html
    '''
    signalOut = QtCore.pyqtSignal(str)

    def __init__(self,parent=None):
        super(MyThread,self).__init__(parent)

        self.identity = None #线程名
        self.flag = True    #定义标志变量，方便从外部退出线程

    def setIdentity(self,text,serialObject):
        self.identity = text
        self.ser=serialObject    #将串口类传入线程，执行串口读取
        #print self.identity

    def run(self):
        '''
        线程开启后自动运行该函数, 直到flag标志为false, 函数自动退出，线程结束
        '''
        while self.flag:
            if self.ser.isOpen():
                try:
                    Bytes = self.ser.in_waiting #读取串口缓冲区中字节数
                    if Bytes>0:
                        data = self.ser.read(Bytes)#将缓冲区中字节全部读出
                        self.signalOut.emit(data)
                        #data是str类型, 但被槽函数接收之后变成了QString类型
                except Exception as e:
                    pass
                    #print e


        



class SoundCollector(QtGui.QMainWindow):
    '''
    窗体类, 继承自QtGui.QMainWindow
    '''
    def __init__(self, parent=None):
        super(SoundCollector, self).__init__()
        #QtGui.QMainWindow.__init__(self) #another way to initialize
        self.flag=True
        self.s = serial.Serial()#新建一个串口Object
        self.s.timeout = 1#1s未读取到串口数据则超时退出
        self.wave = np.array([])#存储所有串口接收到的字符
        

        self.Startbutton = QtGui.QPushButton("Start", self)
        self.Startbutton.move(30, 50)

        self.Stopbutton = QtGui.QPushButton("Stop", self)
        self.Stopbutton.move(150, 50)

        self.OnOffSerialPortLabel = QtGui.QLabel(u'开/关', self)
        self.OnOffSerialPortLabel.move(30, 200)
        self.OnOffSerialPortButton = QtGui.QPushButton("Open", self)
        self.OnOffSerialPortButton.move(120, 200)

        self.serialportLabel = QtGui.QLabel(u'串口号', self)
        self.serialportLabel.move(30, 250)
        self.serialportComboBox = CustomComboBox(self)
        self.serialportComboBox.move(120,250)
        
        self.baudrateLabel = QtGui.QLabel(u'波特率', self)
        self.baudrateLabel.move(30, 300)  
        self.baudrateComboBox = QtGui.QComboBox(self)
        self.baudrateComboBox.addItems(["115200", "9600", "57600"])
        self.baudrateComboBox.ItemIndex = 2
        self.baudrateComboBox.move(120,300)
        self.s.baudrate = 115200 #default param


        self.bytesizeLabel = QtGui.QLabel(u'数据位', self)
        self.bytesizeLabel.move(30, 350)  
        self.bytesizeComboBox = QtGui.QComboBox(self)
        self.bytesizeComboBox.addItems(["8", "7", "6", "5"])
        self.bytesizeComboBox.move(120,350)
        self.s.bytesize = serial.EIGHTBITS

        self.stopbitsLabel = QtGui.QLabel(u'停止位', self)
        self.stopbitsLabel.move(30, 400)  
        self.stopbitsComboBox = QtGui.QComboBox(self)
        self.stopbitsComboBox.addItems(["1", "1.5", "2"])
        self.stopbitsComboBox.move(120,400)
        self.s.stopbits = serial.STOPBITS_ONE
        
        self.parityLabel = QtGui.QLabel(u'校验位', self)
        self.parityLabel.move(30, 450)  
        self.parityComboBox = QtGui.QComboBox(self)
        self.parityComboBox.addItems(["None", "Odd", "Even", "Mark", "Space"])
        self.parityComboBox.move(120,450)
        self.s.parity = serial.PARITY_NONE
        
        self.DispInTextEditradioButton = QtGui.QRadioButton("display", self)
        self.DispInTextEditradioButton.move(30, 100)
        self.DispInTextEditradioButton.setChecked(True)
        
        self.ClearReceiveBufbutton = QtGui.QPushButton("Clear", self)
        self.ClearReceiveBufbutton.move(300, 330)

        self.SaveReceiveBufbutton = QtGui.QPushButton("Save", self)
        self.SaveReceiveBufbutton.move(400, 330)
        self.SaveReceiveBufbutton.setEnabled(False)

        self.ReceiveCharradioButton = QtGui.QRadioButton("Char", self)
        self.ReceiveCharradioButton.move(530, 330)
        self.ReceiveHEXradioButton = QtGui.QRadioButton("DEC", self)#此处本应是HEX,暂未实现
        self.ReceiveHEXradioButton.move(620, 330)
        self.ReceiveHEXradioButton.setChecked(False)
        self.ReceiveHEXradioButton.setChecked(True)
        
        self.ReceivebuttonGroup=QtGui.QButtonGroup(self)
        self.ReceivebuttonGroup.addButton(self.ReceiveCharradioButton)
        self.ReceivebuttonGroup.addButton(self.ReceiveHEXradioButton)

        self.ClearTransmitBufbutton = QtGui.QPushButton("Clear", self)
        self.ClearTransmitBufbutton.move(300, 510)

        self.SendTransmitBufbutton = QtGui.QPushButton("Send", self)
        self.SendTransmitBufbutton.move(400, 510)
        self.SendTransmitBufbutton.setEnabled(False)

        self.TransmitCharradioButton = QtGui.QRadioButton("Char", self)
        self.TransmitCharradioButton.move(530, 510)
        self.TransmitCharradioButton.setChecked(True)
        self.TransmitHEXradioButton = QtGui.QRadioButton("HEX", self)
        self.TransmitHEXradioButton.move(620, 510)
        self.TransmitHEXradioButton.setChecked(False)
        
        self.TransmitbuttonGroup=QtGui.QButtonGroup(self)
        self.TransmitbuttonGroup.addButton(self.TransmitCharradioButton)
        self.TransmitbuttonGroup.addButton(self.TransmitHEXradioButton)

        self.ReceiveQTextEdit=QtGui.QTextEdit(self)
        self.ReceiveQTextEdit.move(300, 30)
        self.ReceiveQTextEdit.resize(400, 300)
        #self.ReceiveQTextEdit.setText('hello world\n')
        self.TransmitQTextEdit=QtGui.QTextEdit(self)
        self.TransmitQTextEdit.move(300, 360)
        self.TransmitQTextEdit.resize(400, 150)

        self.connect(self.Startbutton, QtCore.SIGNAL('clicked()'), self.Startbutton_process)        
        self.connect(self.Stopbutton, QtCore.SIGNAL('clicked()'), self.Stopbutton_process)
        self.connect(self.OnOffSerialPortButton, QtCore.SIGNAL('clicked()'), self.OnOffSerialPortButton_process)        
        self.connect(self.ClearReceiveBufbutton, QtCore.SIGNAL('clicked()'), self.ClearReceiveBufbutton_process)        
        self.connect(self.ClearTransmitBufbutton, QtCore.SIGNAL('clicked()'), self.ClearTransmitBufbutton_process)        
        self.connect(self.baudrateComboBox, QtCore.SIGNAL('activated(QString)'),self.baudrateComboBox_Activated)
        self.connect(self.serialportComboBox, QtCore.SIGNAL('activated(QString)'),self.serialportComboBox_Activated)
        self.connect(self.bytesizeComboBox, QtCore.SIGNAL('activated(QString)'),self.bytesizeComboBox_Activated)
        self.connect(self.stopbitsComboBox, QtCore.SIGNAL('activated(QString)'),self.stopbitsComboBox_Activated)
        self.connect(self.parityComboBox, QtCore.SIGNAL('activated(QString)'),self.parityComboBox_Activated)
        
        self.statusBar().showMessage('Ready')
        self.setWindowTitle('SoundCollector')
        self.setWindowIcon(QtGui.QIcon('images/littleStar.ico'))
        self.resize(750, 550)
        self.setFixedSize(750, 550)
        self.center()

        self.t = MyThread()
        self.t.setIdentity("Serial Read Thread", self.s)
        self.t.signalOut.connect(self.addtoText)
        self.t.start()
        thread.start_new_thread(self.DrawWave, ("thread: DrawWave", ))
    def DrawWave(self, threadname):
        while self.flag:        
            if self.s.isOpen():
                try:
                    plt.clf()
                    plt.axis([0,100000,0,255])  
                    if len(self.wave)>100000:
                        plt.plot(self.wave[-100000:-1])
                    else:
                        plt.plot(self.wave)
                    plt.pause(0.001)
                except Exception as e:
                    print e
            else:
                plt.close('all')
                

    def addtoText(self, data):
        '''
        从另一个线程中的信号发射的数据，在这里接收
        将一个字节的QString转换为numpy.array时占据两个字节, 
        第一个字节是对应的uint8类型，第二个字节是0
        '''
        arraydata = np.fromstring(data, dtype=np.uint8)[::2]
        self.wave = np.hstack([self.wave, arraydata])
        if self.DispInTextEditradioButton.isChecked():
            if self.ReceiveHEXradioButton.isChecked():          
                self.ReceiveQTextEdit.append(str(arraydata)[1:-1])
                self.statusBar().showMessage('Display in Decimal!')
            else:
                try:
                    self.ReceiveQTextEdit.append(str(data))
                except Exception as e:
                    self.statusBar().showMessage('encoded wrong!')
                else:
                    self.statusBar().showMessage('Display in Character!')
                
    def closeEvent(self, event):
        '''
        关闭窗口时先将线程的flag标志改成false
        使线程终止
        '''
        self.t.flag=False
        self.flag=False
        time.sleep(0.1)
        sys.exit("goodbye!")

    def center(self):
        #窗体居中
        screen = QtGui.QDesktopWidget().screenGeometry()
        size =  self.geometry()
        self.move((screen.width()-size.width())/2, (screen.height()-size.height())/2)

    def Startbutton_process(self):
        try:
            if self.s.isOpen():
                self.statusBar().showMessage('collecting...')
                self.s.flushInput()
                self.wave = np.array([])
                self.s.write(b'\xa9')
            else:
                QtGui.QMessageBox().question(self, 'msg', "The serial port has not been opened yet", QtGui.QMessageBox.Ok) 
        except Exception as e:
            print e
        
        
        
    def Stopbutton_process(self):
        try:
            if self.s.isOpen():
                self.s.write(b'\xb9')  
                self.statusBar().showMessage("Receive %d bytes"% len(self.wave))
                intwave = self.wave
                # print type(self.wave)  #<class 'PyQt4.QtCore.QString'>
                np.save("wave.npy",intwave)
            else:
                QtGui.QMessageBox().question(self, 'msg', "The serial port has not been opened yet", QtGui.QMessageBox.Ok) 

        except Exception as e:
            print e
        
        #self.ReceiveQTextEdit.append(re.sub(r"(?<=\w)(?=(?:\w\w)+$)", " ", self.wave.encode('hex').split()[0]))
        #print(self.wave)
    def OnOffSerialPortButton_process(self):
        text = self.sender().text()
        try:
            if self.sender().text() == 'Close':
                if self.s.isOpen():                 
                    self.s.close()
                    self.OnOffSerialPortButton.setText('Open')
                    self.serialportComboBox.setEnabled(True)
                    self.baudrateComboBox.setEnabled(True)
                    self.bytesizeComboBox.setEnabled(True)
                    self.stopbitsComboBox.setEnabled(True)
                    self.parityComboBox.setEnabled(True)
                    self.statusBar().showMessage("%s %s successfully!" % (text, self.s.port))
            else:
                if not self.s.isOpen():
                    self.s.open()
                    self.OnOffSerialPortButton.setText('Close')
                    self.serialportComboBox.setEnabled(False)
                    self.baudrateComboBox.setEnabled(False)
                    self.bytesizeComboBox.setEnabled(False)
                    self.stopbitsComboBox.setEnabled(False)
                    self.parityComboBox.setEnabled(False)
                    self.statusBar().showMessage("%s %s successfully!" % (text, self.s.port))
        except Exception as e:
            QtGui.QMessageBox().question(self, 'msg', "fail to open/close port", QtGui.QMessageBox.Ok)
            print e

    def ClearReceiveBufbutton_process(self):
        self.ReceiveQTextEdit.clear()
    def ClearTransmitBufbutton_process(self):
        self.TransmitQTextEdit.clear()

    def baudrateComboBox_Activated(self, text):
        self.s.baudrate=int(text)
        self.statusBar().showMessage("Baudrate : %s "% self.s.baudrate)
    def serialportComboBox_Activated(self, text):
        self.s.port = str(text)
        self.statusBar().showMessage("Serial Port : %s "% self.s.port)
        #print self.s.port
    def bytesizeComboBox_Activated(self, text):
        self.s.bytesize = int(text)
        self.statusBar().showMessage("Bytesize : %s "% self.s.bytesize)
    def stopbitsComboBox_Activated(self, text):
        if text=='1':
            self.s.stopbits = serial.STOPBITS_ONE
        elif text=='1.5':
            self.s.stopbits = serial.STOPBITS_ONE_POINT_FIVE
        elif text=='2':
            self.s.stopbits = serial.STOPBITS_TWO
        self.statusBar().showMessage("Stopbits : %s "% self.s.stopbits)
    def parityComboBox_Activated(self, text):
        if text=="None":
            self.s.parity=serial.PARITY_NONE
        elif text=="Odd":
            self.s.parity=serial.PARITY_ODD
        elif text=="Even":
            self.s.parity=serial.PARITY_EVEN
        elif text=="Mark":
            self.s.parity=serial.PARITY_MARK
        elif text=="Space":
            self.s.parity=serial.PARITY_SPACE
        self.statusBar().showMessage("Parity : %s "% self.s.parity)
    

if __name__=="__main__":
    app = QtGui.QApplication(sys.argv)
    widget = SoundCollector()
    widget.show()
    sys.exit(app.exec_())
/**
 *\*\file ESP32_TF_Control.ino
 *\*\author Sakurashiling
 *\*\HardWareSite https://oshwhub.com/lin_xiandi/autoctrl-wireless-tf
 *\*\License Apache 2.0 署名原作者
 *\*\version v1.5
 **/
#include <WiFi.h>
#include <ESP32WebServer.h>
#include <ESPmDNS.h>

#include "CSS.h" // web、样式文件
#include <SD.h>
#include <SPI.h>

// WIFI的SSID和PASSWORD,可以在下边代码中改创建热点或连接WIFI
char ssid[] = "Myprinter";
char pass[] = "12345678";

// TF卡网页服务
ESP32WebServer server(80);
#define servername "tfserver" // 定义服务器的名字
#define SD_pin 16        // 片选引脚
bool SD_present = false; // 控制TF卡是否存在

/*********  初始化代码段  **********/
void setup(void)
{
    pinMode(32, OUTPUT);
    pinMode(33, OUTPUT);
    digitalWrite(32, LOW); // 模拟开关切换到ESP32X
    digitalWrite(33, LOW); // TF卡电源开
    Serial.begin(115200);  // 串口115200bps

    /*----------创建热点或连接WIFI二选一------------------*/
    WiFi.softAP(ssid, pass); // 生成WIFI热点
    /////////////////////////////////////////////////////
    // WiFi.begin(ssid, pass); // 连接WIFI
    // Serial.print("Connecting");
    // while (WiFi.status() != WL_CONNECTED)
    // {
    //     delay(500);
    //     Serial.print(".");
    // }
    // Serial.println("");
    // Serial.print("Connected,IP adrress:");
    // Serial.println(WiFi.localIP());
    /*--------------------------------------------------*/

    // 设置服务器名称，如果上边定义服务器的名字为tfserver，则可以使用 http://tfserver.local/ 进入。
    if (!MDNS.begin(servername))
    {
        Serial.println(F("Error setting up MDNS responder!"));
        ESP.restart();
    }

    /********* 服务器命令 **********/
    server.on("/", SD_dir);
    server.on("/upload", File_Upload);
    server.on("/on", TF_Connect);
    server.on("/off", TF_Disconnect);
    server.on(
        "/fupload", HTTP_POST, []()
        { server.send(200); },
        handleFileUpload);

    server.begin();

    Serial.println("HTTP server started");
}

/*********  循环代码段  **********/

void loop(void)
{
    server.handleClient(); // 监听客户端连接
}

/********* 功能 **********/
// 服务器web的初始页面，列出目录并给你删除和上传的机会
void SD_dir()
{
    if (SD_present)
    {
        // 本段代码由前辈MC2022首次编写，包含下载或删除的操作
        if (server.args() > 0) // 接收到参数，如果没有参数则忽略
        {
            Serial.println(server.arg(0));

            String Order = server.arg(0);
            Serial.println(Order);

            if (Order.indexOf("download_") >= 0)
            {
                Order.remove(0, 9);
                SD_file_download(Order);
                Serial.println(Order);
            }

            if ((server.arg(0)).indexOf("delete_") >= 0)
            {
                Order.remove(0, 7);
                SD_file_delete(Order);
                Serial.println(Order);
            }
        }

        File root = SD.open("/");
        if (root)
        {
            root.rewindDirectory();
            SendHTML_Header();
            webpage += F("<table align='center'>");
            webpage += F("<tr><th>文件名/类型</th><th style='width:25%'>文件类型/目录名</th><th>文件大小</th></tr>");
            printDirectory("/", 0);
            webpage += F("</table>");
            SendHTML_Content();
            root.close();
        }
        else
        {
            SendHTML_Header();
            webpage += F("<h3>找不到文件</h3>");
        }
        append_page_footer();
        SendHTML_Content();
        SendHTML_Stop(); // 需要停止，因为没有发送内容长度
    }
    else
        ReportSDNotPresent();
}

// 将文件上传到 TF卡
void File_Upload()
{
    append_page_header(SD_present);
    webpage += F("<h3>选择要上传的文件</h3>");
    webpage += F("<FORM action='/fupload' method='post' enctype='multipart/form-data'>");
    webpage += F("<input class='buttons' style='width:45%' type='file' name='fupload' id = 'fupload' value=''>"); // 上传页面按钮区域
    webpage += F("<button class='buttons' style='width:10%' type='submit'>上传</button><br><br>");                // 上传页面按钮宽度
    webpage += F("<h3>点击上传按钮后请耐心等待,不要离开界面!</h3>");
    webpage += F("<a href='/'>[返回]</a><br><br>");
    append_page_footer();
    server.send(200, "text/html", webpage);
}

// 将TF卡连接到ESP32
void TF_Connect()
{
    digitalWrite(33, HIGH); // TF卡电源关
    delay(10);
    digitalWrite(32, HIGH); // 模拟开关切换到ESP32
    digitalWrite(33, LOW);  // TF卡电源开
    delay(10);
    Serial.print(F("Initializing TF card..."));
    if (!SD.begin(SD_pin))
    {
        Serial.println(F("Card failed or not present, no TF Card data logging possible..."));
        SD_present = false;
    }
    else
    {
        Serial.println(F("Card initialised... file access enabled..."));
        SD_present = true;
    }
    append_page_header(SD_present);
    if (SD_present)
    {
        webpage += F("<h3>TF卡已经连接,请等待3S再尝试读取文件列表</h3>");
    }
    else
    {
        webpage += F("<h3>TF卡未连接成功,请重试!</h3>");
        webpage += F("<h2>Tips:烧录接口不会给TF卡供电!</h2>");
    }
    webpage += F("<a href='/'>[返回]</a><br><br>");
    append_page_footer();
    server.send(200, "text/html", webpage);
}
// 将TF卡连接到外部设备(3D打印机)
void TF_Disconnect()
{
    digitalWrite(33, HIGH); // TF卡电源关
    delay(10);
    digitalWrite(32, LOW); // 模拟开关切换到外部设备
    digitalWrite(33, LOW); // TF卡电源开
    delay(10);
    Serial.println(F("Card failed or not present, no TF Card data logging possible..."));
    SD_present = false;

    append_page_header(SD_present);
    webpage += F("<h3>TF卡已经断开</h3>");
    webpage += F("<a href='/'>[返回]</a><br><br>");
    append_page_footer();
    server.send(200, "text/html", webpage);
}

// 打印目录，它在 void SD_dir() 中调用
void printDirectory(const char *dirname, uint8_t levels)
{

    File root = SD.open(dirname);

    if (!root)
    {
        return;
    }
    if (!root.isDirectory())
    {
        return;
    }
    File file = root.openNextFile();

    int i = 0;
    while (file)
    {
        if (webpage.length() > 1000)
        {
            SendHTML_Content();
        }
        if (file.isDirectory())
        {
            webpage += "<tr><td>" + String(file.isDirectory() ? "目录" : "File") + "</td><td>" + String(file.name()) + "</td><td></td></tr>";
            printDirectory(file.name(), levels - 1);
        }
        else
        {
            webpage += "<tr><td>" + String(file.name()) + "</td>";
            webpage += "<td>" + String(file.isDirectory() ? "Dir" : "File") + "</td>";
            int bytes = file.size();
            String fsize = "";
            if (bytes < 1024)
                fsize = String(bytes) + " B";
            else if (bytes < (1024 * 1024))
                fsize = String(bytes / 1024.0, 3) + " KB";
            else if (bytes < (1024 * 1024 * 1024))
                fsize = String(bytes / 1024.0 / 1024.0, 3) + " MB";
            else
                fsize = String(bytes / 1024.0 / 1024.0 / 1024.0, 3) + " GB";
            webpage += "<td>" + fsize + "</td>";
            webpage += "<td>";
            webpage += F("<FORM action='/' method='post'>");
            webpage += F("<button type='submit' name='download'");
            webpage += F("' value='");
            webpage += "download_" + String(file.name());
            webpage += F("'>下载</button>");
            webpage += "</td>";
            webpage += "<td>";
            webpage += F("<FORM action='/' method='post'>");
            webpage += F("<button type='submit' name='delete'");
            webpage += F("' value='");
            webpage += "delete_" + String(file.name());
            webpage += F("'>删除</button>");
            webpage += "</td>";
            webpage += "</tr>";
        }
        file = root.openNextFile();
        i++;
    }
    file.close();
}

// 上传一个文件到TF卡中，被void SD_dir()这个函数调用
void SD_file_download(String filename)
{
    if (SD_present)
    {
        File download = SD.open("/" + filename);
        if (download)
        {
            server.sendHeader("Content-Type", "text/text");
            server.sendHeader("Content-Disposition", "attachment; filename=" + filename);
            server.sendHeader("Connection", "close");
            server.streamFile(download, "application/octet-stream");
            download.close();
        }
        else
            ReportFileNotPresent("download");
    }
    else
        ReportSDNotPresent();
}

// 处理文件上传文件到TF卡
File UploadFile;
// 将新文件上传到文件系统
void handleFileUpload()
{
    HTTPUpload &uploadfile = server.upload();
    // 参见 https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/srcv
    // 有关“状态”结构的更多信息，还有其他原因，例如可以使用的传输失败
    if (uploadfile.status == UPLOAD_FILE_START)
    {
        String filename = uploadfile.filename;
        if (!filename.startsWith("/"))
            filename = "/" + filename;
        Serial.print("Upload File Name: ");
        Serial.println(filename);
        SD.remove(filename);                        // 删除以前的版本，否则数据会再次附加到文件中
        UploadFile = SD.open(filename, FILE_WRITE); // 在TF卡中打开用于写入的文件（如果不存在则创建它）
        filename = String();
    }
    else if (uploadfile.status == UPLOAD_FILE_WRITE) // 准备写入文件
    {
        if (UploadFile)
            UploadFile.write(uploadfile.buf, uploadfile.currentSize); // 将接收到的字节写入文件
    }
    else if (uploadfile.status == UPLOAD_FILE_END) // 上传文件结束
    {
        if (UploadFile) // 如果文件创建成功
        {
            UploadFile.close(); // 再次关闭文件
            Serial.print("Upload Size: ");
            Serial.println(uploadfile.totalSize);
            webpage = "";
            append_page_header(SD_present);
            webpage += F("<h3>文件上传成功</h3>");
            webpage += F("<h2>上传的文件: ");
            webpage += uploadfile.filename + "</h2>";
            webpage += F("<h2>文件大小: ");
            webpage += file_size(uploadfile.totalSize) + "</h2><br><br>";
            webpage += F("<a href='/'>[返回]</a><br><br>");
            append_page_footer();
            server.send(200, "text/html", webpage);
        }
        else
        {
            ReportCouldNotCreateFile("upload");
        }
    }
}

// 从TF卡中删除一个文件，被void SD_dir()这个函数调用
void SD_file_delete(String filename)
{
    if (SD_present)
    {
        SendHTML_Header();
        File dataFile = SD.open("/" + filename, FILE_READ); // 现在从TF卡读取数据
        if (dataFile)
        {
            if (SD.remove("/" + filename))
            {
                Serial.println(F("File deleted successfully"));
                webpage += "<h3>文件 '" + filename + "' 已被删除</h3>";
                webpage += F("<a href='/'>[返回]</a><br><br>");
            }
            else
            {
                webpage += F("<h3>错误!文件未被删除</h3>");
                webpage += F("<a href='/'>[返回]</a><br><br>");
            }
        }
        else
            ReportFileNotPresent("delete");
        append_page_footer();
        SendHTML_Content();
        SendHTML_Stop();
    }
    else
        ReportSDNotPresent();
}

//  发送 HTML 标题
void SendHTML_Header()
{
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", ""); // 空内容会抑制 Content-length 标头，因此我们必须自己关闭套接字。
    append_page_header(SD_present);
    server.sendContent(webpage);
    webpage = "";
}

// 发送 HTML 内容
void SendHTML_Content()
{
    server.sendContent(webpage);
    webpage = "";
}

// 发送 HTML 停止
void SendHTML_Stop()
{
    server.sendContent("");
    server.client().stop(); // 因为没有发送内容长度所以停止
}

// 报告TF卡不存在
void ReportSDNotPresent()
{
    SendHTML_Header();
    webpage += F("<h3>TF卡不存在</h3>");
    webpage += F("<a href='/'>[返回]</a><br><br>");
    append_page_footer();
    SendHTML_Content();
    SendHTML_Stop();
}

// 报告文件不存在
void ReportFileNotPresent(String target)
{
    SendHTML_Header();
    webpage += F("<h3>文件不存在</h3>");
    webpage += F("<a href='/");
    webpage += target + "'>[返回]</a><br><br>";
    append_page_footer();
    SendHTML_Content();
    SendHTML_Stop();
}

// 报告无法创建文件
void ReportCouldNotCreateFile(String target)
{
    SendHTML_Header();
    webpage += F("<h3>写入失败 (上传文件为空或未连接或TF卡可能写保护)</h3>");
    webpage += F("<a href='/");
    webpage += target + "'>[返回]</a><br><br>";
    append_page_footer();
    SendHTML_Content();
    SendHTML_Stop();
}

// 文件大小转换
String file_size(int bytes)
{
    String fsize = "";
    if (bytes < 1024)
        fsize = String(bytes) + " B";
    else if (bytes < (1024 * 1024))
        fsize = String(bytes / 1024.0, 3) + " KB";
    else if (bytes < (1024 * 1024 * 1024))
        fsize = String(bytes / 1024.0 / 1024.0, 3) + " MB";
    else
        fsize = String(bytes / 1024.0 / 1024.0 / 1024.0, 3) + " GB";
    return fsize;
}

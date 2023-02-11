/**
 *\*\file CSS.h
 *\*\author Sakurashiling
 *\*\HardWareSite https://oshwhub.com/lin_xiandi/autoctrl-wireless-tf
 *\*\License Apache 2.0 署名原作者
 *\*\version v1.5
 **/
String webpage = ""; // String to save the html code
// flase=未连接 ture=已连接
void append_page_header(bool status)
{
    webpage = F("<!DOCTYPE html><html>");
    webpage = F("<meta charset=\"UTF-8\">"); // 声明编码，防止乱码
    webpage += F("<head>");
    webpage += F("<title>TF卡文件在线管理系统</title>"); // NOTE: 1em = 16px
    webpage += F("<meta name='viewport' content='user-scalable=yes,initial-scale=1.0,width=device-width'>");
    webpage += F("<style>"); // 从这里开始写样式
    webpage += F("body{max-width:65%;margin:0 auto;font-family:arial;font-size:100%;}");
    webpage += F("ul{list-style-type:none;padding:0;border-radius:0.5em;overflow:hidden;background-color:#66ccff;font-size:1em;}");
    webpage += F("li{float:left;border-radius:0em;border-right:0em solid #bbb;}");
    webpage += F("li a{color:white; display: block;border-radius:0.375em;padding:0.44em 0.44em;text-decoration:none;font-size:100%}");
    webpage += F("li a:hover{background-color:#6078eb;border-radius:0.5em;font-size:100%}");
    if (status)
    {
        webpage += F("h1{color:white;border-radius:0.5em;font-size:1.5em;padding:0.2em 0.2em;background:#30cc28;text-align:center;}");
    }
    else
    {
        webpage += F("h1{color:white;border-radius:0.5em;font-size:1.5em;padding:0.2em 0.2em;background:#d97007;text-align:center;}");
    }
    webpage += F("h2{color:blue;font-size:0.8em;}");
    webpage += F("h3{font-size:0.8em;}");
    webpage += F("hend{color:gray;font-size:0.6em;text-align:center;}");
    webpage += F("table{font-family:arial,sans-serif;font-size:0.9em;border-collapse:collapse;width:95%;}"); // css表格
    webpage += F("th,td {border:0.06em solid #dddddd;text-align:center;padding:0.3em;border-bottom:0.06em solid #dddddd;}");
    webpage += F("tr:nth-child(odd) {background-color:#eeeeee;}");
    // webpage += F(".rcorners_n {border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:20%;color:white;font-size:75%;}");
    // webpage += F(".rcorners_m {border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:50%;color:white;font-size:75%;}");
    // webpage += F(".rcorners_w {border-radius:0.5em;background:#558ED5;padding:0.3em 0.3em;width:70%;color:white;font-size:75%;}");
    // webpage += F(".column{float:left;width:50%;height:45%;}");
    // webpage += F(".row:after{content:'';display:table;clear:both;}");
    webpage += F("*{box-sizing:border-box;}");
    webpage += F("a{font-size:75%;}");
    webpage += F("p{font-size:75%;}");
    webpage += F("</style></head>");
    if (status)
    {
        webpage += F("<body><h1>TF卡状态:已连接</h1>");
    }
    else
    {
        webpage += F("<body><h1>TF卡状态:未连接</h1>");
    }
    webpage += F("<ul>");
    webpage += F("<li><a href='/'>文件列表</a></li>"); // 带命令的菜单栏
    webpage += F("<li><a href='/upload'>上传文件</a></li>");
    webpage += F("<li><a href='/on'>连接TF卡</a></li>");
    webpage += F("<li><a href='/off'>断开(TF卡连接外部设备)</a></li>");
    webpage += F("</ul>");
}
void append_page_footer()
{
    webpage += F("<!--B站:(https://space.bilibili.com/15652409)\n南北组贴贴♥♥♥!-->\n</body><br></br><hend>❖Sakura_shiling❖Firmware Ver.1.5❖</hend></html>");
}

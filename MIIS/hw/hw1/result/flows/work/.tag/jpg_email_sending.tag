// 设置电子邮件的相关信息
email_username = 'horizon52183'
email_password = '3e4r5t6y7uSED'
email_to = '2309996590@qq.com'
email_subject = '发票人工审批转发通知'
load ../.txt/jpg_body.txt to email_body
email_attachment_1 = './转人工审批原因.xlsx'
email_attachment_2 = './待人工审批发票.7z'

// 打开 163
https://mail.163.com/

// 重置一下屏幕焦点
keyboard [enter]

// 登录邮箱
// 切换输入法（可能会不同）
keyboard [ctrl][space]
// keyboard `email_username`
keyboard [tab]
keyboard `email_password`
keyboard [enter]

// 点击写信
click //*[text()="写 信"]

// 输入收件人
type //*[@type="text" and @role="combobox"] as `email_to`

// 输入主题
type //div[@aria-label="邮件主题输入框，请输入邮件主题"]//*[@class="nui-ipt-input" and @type="text"] as `email_subject`

// 设置正文的iframe的name属性，以便于后续操作
dom document.getElementsByClassName("APP-editor-iframe")[0].setAttribute("name","email_body_iframe")

// 上传附件
upload .O0 as `email_attachment_1`
upload .O0 as `email_attachment_2`

// 输入正文
frame email_body_iframe
{
    type //*[@class="nui-scroll"] as `email_body`
}

// 点击发送
click //*[text()="发送"]

wait 5s
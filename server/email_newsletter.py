# import required classes
 
from PIL import Image, ImageDraw, ImageFont
import requests
import numpy as np
import statistics
import imghdr

# Import smtplib for the actual sending function
import smtplib 
import secrets
from email.mime.multipart import MIMEMultipart 
from email.mime.text import MIMEText 
from email.mime.base import MIMEBase 
from email import encoders 

# URL = "http://608dev.net/sandbox/sc/garciag/mean_std.py"
# r = requests.get(url=URL)
# t = r.text.strip().split("\n")[0]
# data = eval(t)

# avg_temp = data[0][0]
# avg_humid = data[1][0]
# avg_light = data[2][0]
# avg_moist = data[3][0]

avg_temp = 24.8
avg_humid = 0.95
avg_light = 0.87
avg_moist = 0.3
 
# create Image object with the input image
 
image = Image.open('Berry Smart Newsletter.png')
 
# initialise the drawing context with
# the image object as background
 
draw = ImageDraw.Draw(image)

font = ImageFont.truetype('Roboto-Bold.ttf', size=34)
 
# starting position of the message
 
(x, y) = (200, 540)
message = "Average Temperature is : "+str(avg_temp)
color = 'rgb(255, 255,255)' # white
draw.text((x, y), message, fill=color, font=font)

(x, y) = (200, 700)
name = 'Average Humidity is : '+str(avg_humid)
color = 'rgb(255, 255, 255)' # white color
draw.text((x, y), name, fill=color, font=font)

(x, y) = (200, 850)
name = 'Average Light is : '+str(avg_light)
color = 'rgb(255, 255, 255)' # white color
draw.text((x, y), name, fill=color, font=font)

(x, y) = (200, 1000)
name = 'Average Soil Moisture is : '+str(avg_moist)
color = 'rgb(255, 255, 255)' # white color
draw.text((x, y), name, fill=color, font=font)
 
# save the edited image
 
image.save('newsletter.png', optimize = True, quality = 20)

#Emailing Code:
fromaddr = secrets.fromaddr
toaddr = secrets.toaddr
   
# instance of MIMEMultipart 
msg = MIMEMultipart() 
  
# storing the senders email address   
msg['From'] = fromaddr 
  
# storing the receivers email address  
msg['To'] = toaddr 
  
# storing the subject  
msg['Subject'] = 'The Berry Smart Daily Digest!'
  
# string to store the body of the mail 
body = "Here is your Berry Smart Daily Digest : "
  
# attach the body with the msg instance 
msg.attach(MIMEText(body, 'plain')) 
  
# open the file to be sent  
filename = 'newsletter.png'
attachment = open('newsletter.png', "rb") 
  
# instance of MIMEBase and named as p 
p = MIMEBase('application', 'octet-stream') 
  
# To change the payload into encoded form 
p.set_payload((attachment).read()) 
  
# encode into base64 
encoders.encode_base64(p) 
   
p.add_header('Content-Disposition', "attachment; filename= %s" % filename) 
  
# attach the instance 'p' to instance 'msg' 
msg.attach(p) 
  
# creates SMTP session 
s = smtplib.SMTP('smtp.gmail.com', 587) 
  
# start TLS for security 
s.starttls() 
  
# Authentication 
s.login(fromaddr, secrets.password) 
  
# Converts the Multipart msg into a string 
text = msg.as_string() 
  
# sending the mail 
s.sendmail(fromaddr, toaddr, text) 
  
# terminating the session 
s.quit() 


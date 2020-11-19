
# -*- coding: utf-8 -*-
from urllib.request import urlopen, Request
import urllib
import bs4
import firebase_admin
from firebase_admin import credentials
from firebase_admin import db
import time
from datetime import datetime
from pytz import timezone
from pyfirmata import Arduino
import string

# firebase 인증
cred = credentials.Certificate('mykey.json')
firebase_admin.initialize_app(cred, {'databaseURL': 'https://project-d8544.firebaseio.com'})

ref = db.reference()  # db위치 지정

# 강수량 받아오는 url
url = 'http://www.kweather.co.kr/forecast/forecast_lifestyle_detail.html?main_map=1&area1=area2&area2=27110000_143'
# 자외선지수 받아오는 url
url1 = 'https://www.weather.go.kr/weather/lifenindustry/li_asset/popup/imgdata_popup.jsp?CODE=A07_1&point=1'

# 특정 주기로 계속해서 크롤링, firebase 업데이트
while (True):
    # 기상정보 DB에 저장
    req = Request(url)
    req1 = Request(url1)

    page = urlopen(req)
    page1 = urlopen(req1)

    html = page.read()
    html1 = page1.read()

    soup = bs4.BeautifulSoup(html, 'html5lib')
    soup1 = bs4.BeautifulSoup(html1, 'html5lib')

    rain = soup.find('li', class_='lifestyle_present_forecast_table').select_one("td:nth-of-type(5)").text
    sun = soup1.find('table', class_='viewDataT').find('td').text

    rainint = rain[:-2]  # 뒤에 mm 제거 후 저장
    sunint = int(해)
    if (rainint == '-'):
        rainint = 0

    # 현재시각
    dt = datetime.now(timezone('Asia/Seoul'))
    # 현재시각 출력 형태
    dtf = dt.strftime('%Y-%m-%d %H:%M:%S')

    # 오후7시 이후와 오전 10시 전에는 자외선지수 0으로 설정
    if (dt.hour > 18 or dt.hour < 11):
        sunint = 0

    ref = db.reference('Switch')
    switch = ref.child('value').get()  # 수동개폐스위치:자동이면OFF 수동이면ON

    ref = db.reference('Status')
    status = ref.child('value').get()  # 현재상태:닫혀있으면OFF 열려있으면ON

    print('현재시각: ' + str(dtf))

    # 자동상태(수동모드 OFF상태)에서
    # 강수량은 10 이상이면 높음 -> 펼침
    # 자외선지수는 6 이상이면 높음 -> 펼침
    # 위 두가지 해당사항 없으면 -> 접힘
    if ('OFF' in switch):
        if (float(rainint) < 10):
            print('현재 비가 안옵니다.')
            status = '0'
        if (sunint < 6):
            print('현재 자외선지수가 낮습니다.')
            status = '0'
        if (float(rainint) >= 10):
            print('현재 비가 옵니다.')
            status = '1'
        if (sunint >= 6):
            print('현재 자외선지수가 높습니다.')
            status = '1'
        ref.update({'value': status})
        print('자동개폐모드')

    if ('ON' in switch):
        print('수동개폐모드')

    print('강수량: ' + str(rainint))
    print('자외선: ' + str(sunint))
    print('현재상태: ' + status)
    print('\n')

    # 크롤링해온 강수량과 자외선지수 firebase에 업데이트
    ref = db.reference('Rain')
    ref.update({'value': rainint})
    ref = db.reference('Sun')
    ref.update({'value': sunint})

    time.sleep(0.5)

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    agreement/method.cpp \
    clock.cpp \
    main.cpp \
    memo/showjsontextedit.cpp \
    onemonitor.cpp \
    weather.cpp

HEADERS += \
    agreement/method.h \
    clock.h \
    memo/showjsontextedit.h \
    onemonitor.h \
    weather.h

FORMS += \
    onemonitor.ui \
    weather.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    img_weather/type/BaoXue.png \
    img_weather/type/BaoYu.png \
    img_weather/type/BaoYuDaoDaBaoYu.png \
    img_weather/type/DaBaoYu.png \
    img_weather/type/DaBaoYuDaoTeDaBaoYu.png \
    img_weather/type/DaDaoBaoXue.png \
    img_weather/type/DaDaoBaoYu.png \
    img_weather/type/DaXue.png \
    img_weather/type/DaYu.png \
    img_weather/type/DongYu.png \
    img_weather/type/DuoYun.png \
    img_weather/type/FuChen.png \
    img_weather/type/LeiZhenYu.png \
    img_weather/type/LeiZhenYuBanYouBingBao.png \
    img_weather/type/Mai.png \
    img_weather/type/QiangShaChenBao.png \
    img_weather/type/Qing.png \
    img_weather/type/ShaChenBao.png \
    img_weather/type/TeDaBaoYu.png \
    img_weather/type/Wu.png \
    img_weather/type/XiaoDaoZhongXue.png \
    img_weather/type/XiaoDaoZhongYu.png \
    img_weather/type/XiaoXue.png \
    img_weather/type/XiaoYu.png \
    img_weather/type/Xue.png \
    img_weather/type/YangSha.png \
    img_weather/type/Yin.png \
    img_weather/type/Yu.png \
    img_weather/type/YuJiaXue.png \
    img_weather/type/ZhenXue.png \
    img_weather/type/ZhenYu.png \
    img_weather/type/ZhongDaoDaXue.png \
    img_weather/type/ZhongDaoDaYu.png \
    img_weather/type/ZhongXue.png \
    img_weather/type/ZhongYu.png \
    img_weather/type/undefined.png

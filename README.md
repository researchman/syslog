# syslog

一、简介

​	基于boostlog的简单日志库，实现日志写文件的操作。

二、功能

​	1.主要支持三个级别的输出，分别是info，warn，error；

​	2.日志文件支持按大小自动切分，按零点切分，自动回滚，文件超过指定大小自动切新文件；

​	3.日志格式支持显示时间，级别，文件名，行号等信息的显示；
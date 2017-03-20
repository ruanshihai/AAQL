# AAQL

## 项目介绍

信息抽取是指从非结构化或半结构化数据中提取结构化信息的任务。

AAQL是一个基于规则的信息抽取系统，系统原型参考[IBM SystemT](http://researcher.watson.ibm.com/researcher/view_group.php?id=1264)。

系统核心为一种语法类似SQL的标记查询语言（[AQL](https://www.ibm.com/support/knowledgecenter/en/SSPT3X_4.0.0/com.ibm.swg.im.infosphere.biginsights.aqlref.doc/doc/aql-overview.html)的变种），通过AQL声明的规则从文本中提取符合给定模式的信息(比如:从体育新闻报道中提取赛事结果)。基于编译原理,实现该语言的编译器,生成中间表示,再通过解析器在输入文本中执行信息抽取的具体过程。

## 演示

从大量新闻报道中提取足球赛事结果（时间、双方球队名、比分）：

    任务定义：aql/Soccer.aql
    输入文本：input/news-0-0.txt
    字典文件：dict/soccer/*
    示例输出：output/Soccer.output

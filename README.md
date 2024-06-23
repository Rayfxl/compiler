# UESTC编译原理课程实验

要求实现一个极小语言的词法分析与语法分析程序。语言如下：

```
begin

 integer k;

 integer function F(n);

  begin

   integer n;

   if n<=0 then F:=1

   else F:=n*F(n-1)

  end;

 read(m);

 k:=F(m);

 write(k)

end
```

​	实验源代码使用C++编写，分为三个部分，其中词法分析部分为lexical_analyzer.h和lexical_analyzer.cpp，采用面向过程的思想。语法分析部分主要实现在了grammar_analyzer.h中，采用面向对象的思想。主控程序为main.cpp，实现了调用分析的流程。
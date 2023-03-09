#判断年纪  
#!/bin/bash  
read -p "Please input your age: " age  
if [[ $age =~ [^0-9] ]] ;then  
    echo "please input a int"  
    exit 10  
elif [ $age -ge 150 ];then  
    echo "your age is wrong"  
    exit 20  
elif [ $age -gt 18 ];then  
    echo "good good work,day day up"  
else  
    echo "good good study,day day up"  
fi 

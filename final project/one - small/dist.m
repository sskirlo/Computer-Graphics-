function [ val ] = dist( c,t )

    delta=c(1,1);
    told=c(1,2);
    
    %if(t<told)
    %   val=1; 
    %else
       val=(t-told)^2+(t^2-told^2)^2-delta^2; 
    %end

end


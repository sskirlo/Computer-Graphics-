
%applies box filter to water pattern to remove sharp annoying features 

function [h] = myfilter(h1)

    sk=3; %size of averaging window on each side
    
    s=size(h1);
    N=s(1,1);
    M=s(1,2); 
    
    h=h1;
    
    for n=1:1:N
        for m=1:1:M
            sum=0;
            count=0;
            for nk=-sk:1:sk
                for mk=-sk:1:sk
                    %check if in window
                    if( (1<(n+nk)) && ((n+nk)<(N+1)) && (1<(m+mk)) && ((m+mk)<(M+1)) )
                        sum=sum+h1(n+nk,m+mk);
                        count=count+1;
                    end
                end
            end
           h(n,m)=sum/count; 
        end
    end
    

end


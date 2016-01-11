function [ h ] = conserveVolume(h1,hnew,b )

    %function conserves water in 1D

    s=size(b);
    N=s(1,1);

    %determine location of patches of water
    
    startsold=[];
    endsold=[];
    countsold=0; 
    
    inwater=0;
    for n=1:1:N
        if((h1(n,1)>b) & (inwater==0))
            inwater=1;
            startsold=[startsold,n];
        end
        
        if( ((h1(n,1)<b) ) & (inwater==1))
            inwater=0;
            endsold=[endsold,n-1]; 
            countsold=countsold+1; 
        end
        
        if( (n==N) & (inwater==1))
            inwater=0;
            endsold=[endsold,n]; 
            countsold=countsold+1; 
        end
        
    end
    
    startsnew=[];
    endsnew=[];
    countsnew=0; 
    
    inwater=0;
    for n=1:1:N
        if(hnew(n,1)>b & inwater==0)
            inwater=1;
            startsnew=[startsnew,n];
        end
        
        if( ((hnew(n,1)<b) ) & (inwater==1))
            inwater=0;
            endsnew=[endsnew,n-1]; 
            countsnew=countsnew+1; 
        end
        
        if( (n==N) & (inwater==1))
            inwater=0;
            endsnew=[endsnew,n]; 
            countsnew=countsnew+1; 
        end
    end

    %disp('old intervals');
    %disp(startsold);
    %disp(endsold);
    %disp('new intervals');
    %disp(startsnew);
    %disp(endsnew); 
    
    
    %if we have different counts, don't do anything
    if(countsnew~=countsold)
        h=hnew;
        return; 
    end
    
    %try to match up old and new volumes, just compare intervals, 
    %if intervals overlap slightly, we continue, else we return the 
    %old case 
    
    for n=1:1:countsold
        if( ~((startsold(1,n)<endsnew(1,n))| (startsold(1,n)<endsold(1,n))) )
            %if intervals don't overlap we return
            h=hnew;
            return; 
        end
    end
    
    %if intervals overlap and number of puddles is conserved, we correct 
    %the volume for each puddle 
    
    %disp('calculating volumes'); 
    
    for n=1:1:countsold
        volold=0; 
        for nx=startsold(1,n):1:endsold(1,n)
            volold=volold+h1(nx,1)-b(nx,1);
        end
        
        volnew=0; 
        for nx=startsnew(1,n):1:endsnew(1,n)
            volnew=volnew+hnew(nx,1)-b(nx,1);
        end
        
        %display old and new volumes for debugging 
        %disp(n); 
        %disp(volold);
        disp(volnew);
        disp(countsold);
        disp(countsnew);
        
        dh=(-volnew+volold)/(endsnew(1,n)-startsnew(1,n)+1);
        
        %disp('height correction'); 
        %disp(dh);
        hnew(startsnew(1,n):endsnew(1,n),1)=dh+hnew(startsnew(1,n):endsnew(1,n),1); 
        
    end
    
    h=hnew;
    return;
        
end



%we use this to generate control points for chain

close all;
clear all;
clc; 

%create .swp file for chain

res=60; 
inter=3; 


scale=0.40; 
zoff=7.5; 

fp=fopen('chain.swp','w');

radchain=0.2; 

%format for commands
%bsp2 profile 8 11
%[0.000000 -0.250000]

xcir=zeros(1,res);
ycir=zeros(1,res); 

dtheta=2*pi/(res-1); 

fprintf(fp,'bsp2 circle %d %d \n',inter,res+2); 

for n=1:1:res
    xcir(1,n)=radchain*cos(dtheta*n);
    ycir(1,n)=radchain*sin(dtheta*n);
    fprintf(fp,'[%f %f] \n',xcir(1,n)*scale,ycir(1,n)*scale); 
    if(n==1 || n==res)
        fprintf(fp,'[%f %f] \n',xcir(1,n)*scale,ycir(1,n)*scale);  %print 2nd time for b-spline so we get endpoints connected 
    end
end
 %need to repeat last point twice so we can get curve to close with spline, we don't have to 
 %worry about length being 3n+1 with spline

fprintf(fp,'\n'); 

x1=linspace(-0.5,0.5,res); 
y1=linspace(0.5,0.5,res); 

dtheta=pi/(res+1); %define curved part of chain
x2=zeros(1,res);
y2=zeros(1,res); 
for n=1:1:res
    x2(1,n)=0.5*sin(n*dtheta)+0.5;
    y2(1,n)=0.5*cos(n*dtheta);
end

x3=linspace(0.5,-0.5,res); 
y3=linspace(-0.5,-0.5,res);

x4=zeros(1,res);
y4=zeros(1,res); 
for n=1:1:res
    x4(1,n)=-0.5*sin(n*dtheta)-0.5;
    y4(1,n)=-0.5*cos(n*dtheta);
end

xchain=[x1,x2,x3,x4,x1(1)];
ychain=[y1,y2,y3,y4,y1(1)];
zchain=0*xchain;

figure;
hold on; 
%plot3(xchain,ychain,zchain); 
axis equal; 
grid on; 

%now we need to define control curve for repeating chain

%specify distance 
dlink=2-2*radchain; 

%translate by appropriate distance along curve, and produce new control
%points 
told=-4; 
%x=t
%z=t^2

links=20; 
for n=1:1:links
    %calculate appropriate parameter to move correct distance along curve
    tnew=fzero(@(t)dist([dlink,told],t),told+1);
    told=tnew; 
    if(n==11)
       told=told-0.40; %manually fix one of the points, weird near turnaround 
    end
    %define tangent, normal, binormal and vertexs for new point
    T=[1,0,2*told];  %1,2*t
    T=T/norm(T); 
    B=[0,1,0];
    N=cross(B,T); 
    N=N/norm(N); 
    V=[told,0,told^2]; 
    
    %now we apply transformations to curve and display
    
    R=[1,0,0;
       0,0,1;
       0,-1,0];  %rotation matrix for rotating link by 90 degrees, as we translate along curve  
   
    M=[T;N;B]; 
    
    %if
    if(mod(n,2)==0)
        nchain=M'*R'*[xchain;ychain;zchain];
    else
        nchain=M'*[xchain;ychain;zchain];
    end
    s=size(nchain); 
    for n1=1:1:s(1,2)
        nchain(:,n1)=nchain(:,n1)+V(1,:)'; %translate chain position
    end
    
    plot3(nchain(1,:),nchain(2,:),nchain(3,:)-zoff);
    
    fprintf(fp,'bsp3 link%d %d %d \n',n,inter,s(1,2));
    for n1=1:1:s(1,2)
        fprintf(fp,'[%f %f %f] \n',nchain(1,n1)*scale,nchain(2,n1)*scale,(nchain(3,n1)-zoff)*scale);
    end
    fprintf(fp,'\n'); 
    
    %we finally have to specify generalized cylinders to form chain
    %gcyl weird profile sweep

    fprintf(fp,'gcyl linked%d circle link%d \n \n',n,n);
    
end

fclose(fp); 


%make file for just showing link
fp=fopen('link.swp','w');

scale=3; 

fprintf(fp,'bsp2 circle %d %d \n',inter,res+2); 

for n=1:1:res
    xcir(1,n)=radchain*cos(dtheta*n);
    ycir(1,n)=radchain*sin(dtheta*n);
    fprintf(fp,'[%f %f] \n',xcir(1,n)*scale,ycir(1,n)*scale); 
    if(n==1 || n==res)
        fprintf(fp,'[%f %f] \n',xcir(1,n)*scale,ycir(1,n)*scale);  %print 2nd time for b-spline so we get endpoints connected 
    end
end
 %need to repeat last point twice so we can get curve to close with spline, we don't have to 
 %worry about length being 3n+1 with spline

fprintf(fp,'\n'); 

x1=linspace(-0.5,0.5,res); 
y1=linspace(0.5,0.5,res); 

dtheta=pi/(res+1); %define curved part of chain
x2=zeros(1,res);
y2=zeros(1,res); 
for n=1:1:res
    x2(1,n)=0.5*sin(n*dtheta)+0.5;
    y2(1,n)=0.5*cos(n*dtheta);
end

x3=linspace(0.5,-0.5,res); 
y3=linspace(-0.5,-0.5,res);

x4=zeros(1,res);
y4=zeros(1,res); 
for n=1:1:res
    x4(1,n)=-0.5*sin(n*dtheta)-0.5;
    y4(1,n)=-0.5*cos(n*dtheta);
end

xchain=[x1,x2,x3,x4,x1(1)];
ychain=[y1,y2,y3,y4,y1(1)];
zchain=0*xchain;

fprintf(fp,'bsp3 link %d %d \n',inter,s(1,2));
for n1=1:1:s(1,2)
    fprintf(fp,'[%f %f %f] \n',xchain(1,n1)*scale,ychain(1,n1)*scale,zchain(1,n1)*scale);
end
fprintf(fp,'\n'); 
    
%we finally have to specify generalized cylinders to form chain
%gcyl weird profile sweep

fprintf(fp,'gcyl linked circle link \n \n');

fclose(fp); 















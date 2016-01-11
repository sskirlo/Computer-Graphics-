close all;
clear all;
clc; 

N=2;
M=N; 

x=linspace(0,2,N);
y=linspace(0,2,M);
dx=x(1,2)-x(1,1); 
dy=y(1,2)-y(1,1); 

z=zeros(N,M); 

createObj('floor.obj',x,y,z); 
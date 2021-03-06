function [hnew] = computeHnew(b,h0,h1)

    %compute A-matrix for implicit time-stepping
    
    d=h1-b; 
    
    A=sparse(N,N); 
    A(1,1)=1+g*dt^2*(d(1,1)+d(2,1) )/(2*dx^2); 
    A(N,N)=1+g*dt^2*(d(N,1)+d(N-1,1) )/(2*dx^2); 
    
    %fill in other elements along the diagonal
    for nx=2:1:(N-1) 
       A(nx,nx)= 1+g*dt^2*(d(nx-1,1)+2*d(nx,1)+d(nx+1,1) )/(2*dx^2); 
    end
   
    %fill in elements to right and left of diagonal
    for nx=1:1:(N-1)
        A(nx,nx+1)=-g*dt^2*(d(nx,1)+d(nx+1,1))/(2*dx^2); 
        A(nx+1,nx)=-g*dt^2*(d(nx,1)+d(nx+1,1))/(2*dx^2); 
    end
    
    %now we solve for next forward step 
    
    %disp(h0);
    %disp(h1); 
    
    hnew=A\(2*h1-h0); 

end


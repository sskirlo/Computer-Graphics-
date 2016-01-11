
%creates object file from grid, we don't need normals
%we just need verticies from the triangles 
 
function createObj(name,x,y,a)

    s=size(a);
    N=s(1,1);
    M=s(1,2); 

    fp=fopen(name,'w');
    
    %write out verticies
    for n=1:1:N
        for m=1:1:M
            fprintf(fp,'v %f %f %f \n',x(1,n),y(1,m),a(n,m));
        end
    end

    %write out coordinates for forming triangles 
    
    for n=1:1:(N-1)
        for m=1:1:(M-1)
    
            %specify triangles with counter-clockwise orientation, 
            %not sure if this makes a difference for the ray tracer 
            
            %create upper triangle 
            fprintf(fp,'f %d %d %d \n',m+M*(n-1),m+1+M*n,m+M*n);
            
            %create lower triangle 
            fprintf(fp,'f %d %d %d \n',m+M*(n-1),m+1+M*(n-1),m+1+M*n);
            
        end
    end
            
    fclose(fp); 
    
end
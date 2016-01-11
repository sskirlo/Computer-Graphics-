
 for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }


%part of this function was written by Anders Sandberg 
% http://www.aleph.se/Nada/Ray/saveobjmeshgrid.m

function saveobjmesh(name,x,y,z)
% SAVEOBJMESH Save a x,y,z mesh as a Wavefront/Alias Obj file
% SAVEOBJMESH(fname,x,y,z,nx,ny,nz)
%     Saves the mesh to the file named in the string fname
%     x,y,z are equally sized matrices with coordinates.
%     nx,ny,nz are normal directions (optional)

%first compute nx,ny,nz



  l=size(x,1); h=size(x,2);  

  n=zeros(l,h);
  
  fid=fopen(name,'w');
  nn=1;
  for i=1:l
    for j=1:h
      n(i,j)=nn; 
      fprintf(fid, 'v %f %f %f\n',x(i,j),y(i,j),z(i,j)); 
      fprintf(fid, 'vt %f %f\n',(i-1)/(l-1),(j-1)/(h-1)); 
      fprintf(fid, 'vn %f %f %f\n', nx(i,j),ny(i,j),nz(i,j)); 
      nn=nn+1;
    end
  end
  fprintf(fid,'g mesh\n');
  
  for i=1:(l-1)
    for j=1:(h-1)
      if (normals) 
	fprintf(fid,'f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n',n(i,j),n(i,j),n(i,j),n(i+1,j),n(i+1,j),n(i+1,j),n(i+1,j+1),n(i+1,j+1),n(i+1,j+1),n(i,j+1),n(i,j+1),n(i,j+1));
      else
  	fprintf(fid,'f %d/%d %d/%d %d/%d %d/%d\n',n(i,j),n(i,j),n(i+1,j),n(i+1,j),n(i+1,j+1),n(i+1,j+1),n(i,j+1),n(i,j+1));
      end
    end
  end
  fprintf(fid,'g\n\n');
  fclose(fid);

%create movie from png images

name='water';
%name='image'; 
count=234;

% Make an avi movie from a collection of PNG images in a folder.
% Specify the folder.
mydir = '/Users/victoriamessuri/Desktop/Scott/6.837/final project/resultfolder2';

% Open the video writer object.
writerObj = VideoWriter('water2.avi');
% Go through image by image writing it out to the AVI file.
open(writerObj); 
for n = 1:1:count
    fname=[mydir,'/',name,sprintf('%d',n),'.png'];
    fprintf('%s \n',fname);
    A = imread(fname);
    %imshow(imageArray);  % Display image.
    %drawnow; % Force display to update immediately.
    % Write this frame out to the AVI file.
    writeVideo(writerObj, A);
end
% Close down the video writer object to finish the file.
close(writerObj);
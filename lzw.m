%% Lempel-Ziv-Welch

%img = imread('image_name.jpg');
%img = imresize(img,0.25);
%gimg = rgb2gray(img);

gimg = [50,50,200,200;50,50,200,200;50,50,200,200;50,50,200,200;50,50,200,200;50,50,200,200;50,50,200,200;50,50,200,200]
gimgt = gimg';
flattened_img = gimgt(:);
lenimg = size(flattened_img);
lenimg = lenimg(1)

compressed_img = [];
dict = 1;

%% Create a Container 
code = containers.Map('KeyType','char','ValueType','uint64');

%% Assigning the first 256 gray values to first 256 codes
for i=0:255
    code(num2str(i)) = i;
end
next_code = 256;

%% Creating the encoded dictionary
tic
string = '';
prev_code = '';
for i=1:lenimg
    if mod(i,1000)==0
        disp('Compressing...')
    end
    input = num2str(flattened_img(i));
    if isequal('',string)
        string = input;
    else
        prev_string = string;
        string = [string,' ',input]; %#ok<*AGROW>
    end
    
    if ismember(string,keys(code))
        %disp('In Dictionary, Continue...')
        dict = 1;
        continue
    else
        dict = 0;
        %disp('Not in Dictionary...Add to Dictionary...')
        code(string) = next_code;
        next_code = next_code + 1;
        string = input;
    end
    prev_code = code(prev_string);
    compressed_img = [compressed_img,prev_code];
end
disp('Adding the Last Code')
prev_code = code(string);
compressed_img = [compressed_img,prev_code];
toc

%% Evaluation
maxcodeval = max(values(code));
maxbits = length(de2bi(maxcodeval));
comp_img_bytes = length(compressed_img)*maxbits/8;
disp('Compressed Image')
disp(compressed_img)
disp('Highest Value of Code in compressed image')
disp(maxcodeval)
disp('Number of bits required for compression')
disp(maxbits)
disp('Number of bytes in Original Image')
disp(lenimg)
disp('Number of Bytes in Compressed Image')
disp(comp_img_bytes)
disp('Compression Ratio')
disp(comp_img_bytes/lenimg)

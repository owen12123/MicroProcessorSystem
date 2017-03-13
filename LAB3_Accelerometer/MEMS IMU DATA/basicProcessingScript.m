xdata = csvread('newData_X.csv');
ydata = csvread('newData_Y.csv');
zdata = csvread('newData_Z.csv');

ydata(:,2)=-ydata(:,2);

%each set of data has unforced data in :
%xdata : y and z unforced
%ydata : x and z unforced
%zdata : x and y unforced
%take the unforced error and average
ue_x = (abs(mean(ydata(:,1)))+abs(mean(zdata(:,1))))/2;
ue_y = (abs(mean(xdata(:,2)))+abs(mean(zdata(:,2))))/2;
ue_z = (abs(mean(xdata(:,3)))+abs(mean(ydata(:,3))))/2;

%remove baseline averages from all datasets 
xdata = [(xdata(:,1)-ue_x) (xdata(:,2)-ue_y) (xdata(:,3)-ue_z)];
ydata = [(ydata(:,1)-ue_x) (ydata(:,2)-ue_y) (ydata(:,3)-ue_z)];
zdata = [(zdata(:,1)-ue_x) (zdata(:,2)-ue_y) (zdata(:,3)-ue_z)];

%each measurement axis must be 1000mg reading, so find coefficients by
%which the measurements taken are adjusted, first by taking average reading
%when forced and comparing to 1000mg

fr_x = abs(mean(xdata(:,1)));
fr_y = abs(mean(ydata(:,2)));
fr_z = abs(mean(zdata(:,3)));

% find coefficient such that 1000mg = fr_n*coeff
cx = 1000/fr_x;
cy = 1000/fr_y;
cz = 1000/fr_z;

%now each data point can be calculated using the sensor reading sens_n
%using formula: real_reading_n = (sens_n-ue_n)*coeff


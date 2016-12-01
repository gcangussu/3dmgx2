clear variables
close all
clc


load('from_ros.mat')

for strcell = {'green', 'blue', 'red'}
    str = strcell{1};
    load([str '.mat'])
    this = eval(str);
    
    nsamples = size(packs, 2);
    acc = zeros(3, nsamples);
    angr = zeros(3, nsamples);
    mag = zeros(3, nsamples);
    orient = zeros(3, 3, nsamples);
    
    for i = 1:nsamples
        [acc(:,i), angr(:,i), mag(:,i), orient(:,:,i), ~] = ...
            parseAccAngrMagOr(packs(:,i));
    end
    
    acc_ros = this(:, 1:3).';
    angr_ros = this(:, 4:6).';
    
    x = 1:nsamples;
    
    figure
    hold on
    plot(x, acc(1,:), 'b')
    plot(x, acc_ros(1,x), 'c')
    plot(x, acc(2,:), 'r')
    plot(x, acc_ros(2,x), 'm')
    plot(x, acc(3,:), 'g')
    plot(x, acc_ros(3,x), 'y')
    legend('X (MATLAB)', 'X (ROS)', 'Y (MATLAB)', 'Y (ROS)', 'Z (MATLAB)', 'Z (ROS)')
    xlabel('Sample')
    ylabel('Acceleration (g)')
    hold off
    
    figure
    hold on
    plot(x, angr(1,:), 'b')
    plot(x, angr_ros(1,x), 'c')
    plot(x, angr(2,:), 'r')
    plot(x, angr_ros(2,x), 'm')
    plot(x, angr(3,:), 'g')
    plot(x, angr_ros(3,x), 'y')
    legend('X (MATLAB)', 'X (ROS)', 'Y (MATLAB)', 'Y (ROS)', 'Z (MATLAB)', 'Z (ROS)')
    xlabel('Sample')
    ylabel('Angular Rate (rad/s)')
    hold off
    
end
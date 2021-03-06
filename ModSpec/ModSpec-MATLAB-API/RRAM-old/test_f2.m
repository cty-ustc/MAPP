% This script plots the f2 function of RRAM_v0_all with different f2_switch
% values. Then I can adjust default parameters to make them more consistent.

Vs = -1.1:0.02:1.1;
ss = [0, 0.5, 1];

for c = 1:6
	MOD = RRAM_v0_all(5, c);
	MEO = model_exerciser(MOD);
	for d = 1:length(ss)
		Is = MEO.ds_fi(Vs, ss(d), MEO);
		figure(d); plot(Vs, Is); hold on;
	end % d
end % c
for d = 1:length(ss)
	figure(d); grid on; box on; 
	title(sprintf('s=%g', ss(d)));
	xlabel('vpn');
	ylabel('ds');
	legend('f2\_switch=1', 'f2\_switch=2', 'f2\_switch=3', ...
	    'f2\_switch=4', 'f2\_switch=5', 'f2\_switch=6');
end % d

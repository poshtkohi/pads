% Generate a random matrix with elements with normal distributions
% https://sparse.tamu.edu/
filename = 'ecology1.mat';
ll = load(filename, '-mat');
SS = ll.Problem.A;

A = [1 0 0 0;
    0 0 -1 0;
    2 0 1 0;
    0 -1 0 0];

%S = sparse(A);
S = sparse(SS);
[I, J, X] = find(S);

I = I - 1;
J = J - 1;

[m, n] = size(S);
p = zeros(n + 1, 1);
i = zeros(nnz(S), 1);
x = zeros(nnz(S), 1);

%I
%J
%X

%p

for it=1:1:length(J)
    %p(ii + 1) = J(ii)
    for jj=J(it)+2:1:length(p)
        p(jj) = p(jj) + 1;
    end
    i(it) = I(it);
    x(it) = X(it);
    it
end

%p'
%i'
%x'

% Save the matrix size to a file
file = fopen('n.dat', 'w');
fprintf(file, "%d", n);
fclose(file);
% Save the matrix L to a file
writematrix(p, 'p.dat', 'Delimiter', 'tab');
% Save the matrix b to a file
writematrix(i, 'i.dat', 'Delimiter', 'tab');
% Save the matrix b to a file
writematrix(x, 'x.dat', 'Delimiter', 'tab');
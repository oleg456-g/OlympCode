program COLLATZ11(input, output);
(* uses crt; *)
const   PATH_LENGTH = 262;
type    massiv = array [1..PATH_LENGTH] of cardinal;
        path = record length : 0..PATH_LENGTH; numbers : massiv end;
function F(X : cardinal) : cardinal;
begin
      if (X mod 2) = 0 then F := X div 2 else F := 3 * X + 1
end;
procedure APPEND (V : cardinal; var P : PATH);
begin with P do begin
        length := length + 1;
        numbers[length] := V
      end
end;
procedure PATHTO1 (N : cardinal; var P : path);
begin
      P.length := 0;
      while N > 1 do begin
          APPEND(N, P);
          N := F(N);
      end;
        APPEND(1, P)
end;

var  N1, N2, V : cardinal;
     I, J, K, RESULT : cardinal;
     FLAG1, FLAG2 : boolean;
     P1, P2 : path;
begin FLAG1 := false;
      FLAG2 := false;
    readln(N1);
    read(N2);
    if N1=N2 then write(0) else begin
      if N1 > 1 then I := F(N1) else I := N1;
      if N2 > 1 then J := F(N2) else J := N2;
      if (I = N2) or (J = N1) or ((N1 = 4) and (N2 = 1)) or ((N1 = 1) and (N2 = 4)) then
        write(1)
      else begin
        PATHTO1(N1, P1);
        PATHTO1(N2, P2);
        I := P1.length;
        J := P2.length;
        while (I > 0) and (J > 0) and (P1.numbers[I] = P2.numbers[J]) do begin
          I := I - 1;
          J := J - 1;
        end;
        if (I >= 0) and (I < P1.length) then V := P1.numbers[I+1] else
        if (J >= 0) and (J < P2.length) then V := P2.numbers[J+1] else
        V := 1;
        RESULT := I + J;
        if ((N2 = 1) and (N1 > 2)) or ((N1 = 1) and (N2 > 2)) then begin
          RESULT := RESULT - 1;
          FLAG2 := true;
        end;
        writeln(RESULT);
        if I > 1 then begin
          write(P1.numbers[2]);
          for K := 3 to I do if not (FLAG2 and (P1.numbers[K] = 2)) then write(' ', P1.numbers[K]);
          FLAG1 := true
        end;
        if (V > 1) and (V <> N1) and (V <> N2) then
		 if not (FLAG2 and (V = 2)) then begin
          if FLAG1 then write(' ');
          write(V);
          FLAG1 := true
         end;
        if J > 1 then begin
          if FLAG1 then if not (FLAG2 and (P2.numbers[J] = 2)) then write(' ');
          if not (FLAG2 and (P2.numbers[J] = 2)) then write(P2.numbers[J]);
          for K := J-1 downto 2 do begin
           if not (FLAG2 and (P2.numbers[K] <= 4)) then write(' ');
           if not (FLAG2 and (P2.numbers[K] = 2)) then write(P2.numbers[K])
          end;
        end
      end
    end
end.

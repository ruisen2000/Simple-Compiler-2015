signal qtemp: std_logic :='0';
signal qbartemp : std_logic :='0';
signal S1 : std_logic_vector(3 downto 0);
signal S2 : std_logic :='0';
signal S3 : std_logic :='0';
signal S4 : std_logic_vector(3 downto 0);


if('1' = '1')   then         --Reset the output.
 qtemp <= '0';
 qbartemp <= '1';
elsif( rising_edge(clk) ) then
if(J<='0' and K>='0') then       --No change in the output
 NULL;
elsif (S1>'0' and S3<'1')   then  --Set the output.
 qtemp <= '0';
 qbartemp <= '1';
elsif(S4="00" and s1="0000")  then   --Reset the output.
 qtemp <= '1';
 qbartemp <= '0';
else                           --Toggle the output.
 qtemp <= not qtemp;
 qbartemp <= not qbartemp;
end if;
end if;
end if;
end process;

end Behavioral;

--Begin
SELECT ����=case when a.colorder=1 then d.name else '' end,
��˵��=case when a.colorder=1 then isnull(f.value,'') else '' end,
�ֶ����=a.colorder,
�ֶ���=a.name,
��ʶ=case when COLUMNPROPERTY( a.id,a.name,'IsIdentity')=1 then '��'else '' end,
����=case when exists(SELECT 1 FROM sysobjects where xtype='PK' and parent_obj=a.id and name in (
SELECT name FROM sysindexes WHERE indid in(
SELECT indid FROM sysindexkeys WHERE id = a.id AND colid=a.colid
))) then '��' else '' end,
����=b.name,
ռ���ֽ���=a.length,
����=COLUMNPROPERTY(a.id,a.name,'PRECISION'),
С��λ��=isnull(COLUMNPROPERTY(a.id,a.name,'Scale'),0),
�����=case when a.isnullable=1 then '��'else '' end,
Ĭ��ֵ=isnull(e.text,''),
�ֶ�˵��=isnull(g.[value],'')
FROM syscolumns a
left join systypes b on a.xusertype=b.xusertype
inner join sysobjects d on a.id=d.id  and d.xtype='U' and  d.name<>'dtproperties'
left join syscomments e on a.cdefault=e.id
left join sysproperties g on a.id=g.id and a.colid=g.smallid
left join sysproperties f on d.id=f.id and f.smallid=0
--where d.name=@ConfigurationTablName
order by a.id,a.colorder
--END
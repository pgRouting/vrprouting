DROP SCHEMA IF EXISTS homberger_1000_c1_10_1 CASCADE;
CREATE SCHEMA homberger_1000_c1_10_1;

SET search_path TO 'homberger_1000_c1_10_1', 'public';

CREATE OR REPLACE FUNCTION homberger_1000_c1_10_1.coord_to_id(x INTEGER, y INTEGER)
RETURNS BIGINT
AS $BODY$
BEGIN
  RETURN CONCAT(x, LPAD(y::TEXT, 4, '0'))::BIGINT;
END;
$BODY$ LANGUAGE plpgsql IMMUTABLE STRICT;


DROP TABLE IF EXISTS homberger_1000_c1_10_1.jobs;
CREATE TABLE homberger_1000_c1_10_1.jobs (
  id              INTEGER   NOT NULL PRIMARY KEY,
  location_index  BIGINT    NOT NULL GENERATED ALWAYS AS (coord_to_id(x, y)) STORED,
  x               INTEGER   NOT NULL,
  y               INTEGER   NOT NULL,
  delivery        INTEGER   NOT NULL,
  service         INTEGER   NOT NULL,
  tw_open         INTEGER   NOT NULL,
  tw_close        INTEGER   NOT NULL
);


COPY homberger_1000_c1_10_1.jobs
(id, x, y, delivery, service, tw_open, tw_close) FROM stdin;
0	250	250	0	0	0	1824
1	387	297	10	90	200	270
2	5	297	10	90	955	1017
3	355	177	20	90	194	245
4	78	346	30	90	355	403
5	286	159	20	90	530	597
6	322	465	10	90	226	291
7	393	408	30	90	630	708
8	89	216	10	90	495	562
9	76	345	30	90	539	586
10	410	285	20	90	499	556
11	472	189	30	90	385	440
12	270	49	30	90	925	970
13	219	325	10	90	520	563
14	437	12	20	90	557	613
15	418	218	20	90	974	1030
16	20	488	10	90	754	820
17	77	347	20	90	436	505
18	73	346	10	90	726	772
19	480	455	10	90	1123	1179
20	129	292	10	90	552	610
21	337	257	20	90	805	863
22	237	254	20	90	629	688
23	131	220	30	90	749	810
24	417	417	10	90	954	1020
25	287	143	10	90	748	793
26	379	80	20	90	737	789
27	87	285	30	90	709	764
28	374	489	20	90	269	339
29	440	247	20	90	717	779
30	281	137	10	90	370	416
31	379	196	20	90	665	714
32	297	102	10	90	214	281
33	39	76	10	90	328	399
34	35	306	20	90	369	440
35	435	68	10	90	259	314
36	245	251	10	90	60	130
37	409	88	20	90	655	715
38	273	55	20	90	196	246
39	163	340	30	90	468	513
40	129	223	20	90	274	340
41	371	200	10	90	843	916
42	357	26	20	90	1162	1202
43	470	473	10	90	1221	1270
44	77	344	10	90	254	319
45	478	461	10	90	462	529
46	128	291	10	90	639	706
47	111	113	10	90	632	694
48	164	21	20	90	578	639
49	21	485	30	90	487	540
50	108	26	10	90	423	491
51	424	286	10	90	517	571
52	200	270	20	90	208	260
53	7	300	30	90	1056	1104
54	433	31	10	90	285	333
55	420	295	20	90	976	1044
56	451	367	20	90	1134	1195
57	118	157	20	90	403	465
58	489	101	20	90	1005	1066
59	200	261	10	90	945	1017
60	476	483	10	90	657	722
61	30	302	10	90	572	632
62	8	300	10	90	1131	1211
63	275	45	20	90	444	515
64	238	46	40	90	735	784
65	152	206	10	90	910	973
66	99	218	30	90	154	205
67	393	410	20	90	913	982
68	480	460	20	90	549	627
69	337	256	20	90	154	201
70	389	300	10	90	293	364
71	90	237	20	90	1229	1284
72	321	280	30	90	143	194
73	417	218	10	90	1060	1126
74	64	108	10	90	473	539
75	57	432	10	90	973	1033
76	356	24	20	90	249	323
77	143	207	30	90	267	326
78	221	325	10	90	594	673
79	22	483	30	90	325	389
80	24	65	10	90	904	976
81	141	431	30	90	362	423
82	202	9	10	90	1225	1281
83	20	48	10	90	1294	1358
84	457	163	20	90	284	346
85	470	290	10	90	835	895
86	268	400	20	90	869	928
87	337	251	10	90	145	210
88	366	20	10	90	874	927
89	72	344	20	90	618	695
90	149	203	10	90	553	597
91	55	428	20	90	511	568
92	189	7	30	90	310	372
93	397	18	10	90	519	575
94	481	122	30	90	607	653
95	20	73	10	90	290	354
96	54	430	20	90	610	652
97	74	350	10	90	915	955
98	202	8	10	90	1134	1190
99	406	285	20	90	412	456
100	460	2	20	90	655	720
101	441	244	10	90	909	955
102	1	293	10	90	492	554
103	115	465	10	90	409	465
104	422	420	20	90	490	545
105	75	347	10	90	810	872
106	141	428	20	90	208	266
107	404	264	20	90	395	462
108	398	23	10	90	270	334
109	148	211	20	90	167	233
110	362	22	20	90	1050	1121
111	479	127	20	90	412	478
112	443	246	20	90	536	593
113	22	37	10	90	833	894
114	65	100	10	90	763	811
115	404	447	20	90	250	280
116	102	264	10	90	148	215
117	444	271	10	90	633	688
118	325	466	40	90	670	717
119	124	219	20	90	571	615
120	335	243	20	90	428	495
121	26	42	20	90	455	519
122	113	106	20	90	198	260
123	399	450	20	90	971	1056
124	146	439	10	90	642	707
125	162	336	20	90	180	247
126	2	295	30	90	774	828
127	122	35	10	90	305	376
128	346	60	10	90	1027	1086
129	93	213	30	90	409	457
130	218	316	20	90	73	134
131	116	156	10	90	495	557
132	419	422	20	90	575	647
133	467	141	10	90	574	637
134	187	491	20	90	771	854
135	433	25	20	90	903	970
136	286	137	10	90	459	516
137	370	82	40	90	450	509
138	132	290	10	90	818	898
139	435	11	40	90	362	422
140	206	261	10	90	1050	1104
141	428	291	10	90	698	764
142	95	384	30	90	719	785
143	205	223	20	90	491	544
144	463	140	10	90	295	366
145	343	47	20	90	738	805
146	435	62	30	90	1156	1218
147	387	488	20	90	987	1046
148	121	157	10	90	220	279
149	182	463	30	90	372	440
150	92	216	10	90	777	834
151	370	83	20	90	357	419
152	444	242	10	90	441	501
153	90	285	40	90	981	1042
154	446	64	10	90	600	665
155	312	264	20	90	131	179
156	208	11	20	90	1316	1382
157	442	247	10	90	623	689
158	140	431	30	90	280	324
159	118	153	30	90	949	1028
160	134	428	20	90	935	992
161	91	235	10	90	1140	1188
162	406	257	10	90	596	637
163	377	76	20	90	638	699
164	474	96	30	90	271	327
165	346	57	20	90	925	1002
166	393	301	30	90	482	545
167	412	214	10	90	165	215
168	210	398	10	90	592	647
169	346	150	20	90	948	998
170	62	453	10	90	515	584
171	437	61	10	90	1063	1127
172	87	96	30	90	744	810
173	367	83	30	90	266	324
174	91	268	10	90	869	931
175	459	145	40	90	233	291
176	63	444	20	90	1084	1146
177	433	27	20	90	628	694
178	60	454	10	90	892	952
179	478	99	10	90	617	675
180	485	454	10	90	842	890
181	6	292	10	90	316	360
182	398	20	20	90	333	394
183	248	251	20	90	2	63
184	433	15	20	90	297	344
185	64	426	10	90	1162	1223
186	154	207	20	90	1002	1066
187	481	456	40	90	1028	1091
188	380	498	10	90	523	582
189	88	286	10	90	795	859
190	111	23	10	90	266	326
191	199	187	20	90	433	490
192	269	48	20	90	823	889
193	367	178	20	90	570	620
194	452	163	30	90	219	263
195	431	28	20	90	724	783
196	8	116	20	90	894	957
197	233	47	20	90	356	415
198	237	43	20	90	631	702
199	320	262	10	90	585	656
200	188	4	20	90	413	455
201	180	6	10	90	774	839
202	328	466	30	90	847	906
203	393	12	10	90	617	671
204	409	90	20	90	569	617
205	406	87	10	90	936	999
206	394	407	20	90	733	789
207	358	183	20	90	932	1005
208	186	461	20	90	220	284
209	38	72	30	90	420	496
210	323	471	30	90	469	546
211	224	192	10	90	215	274
212	278	46	20	90	205	269
213	272	403	30	90	494	555
214	232	42	10	90	545	599
215	18	63	10	90	716	792
216	58	458	10	90	702	774
217	416	417	20	90	1057	1099
218	205	230	30	90	203	263
219	65	453	30	90	423	490
220	421	294	10	90	883	953
221	434	245	10	90	1377	1416
222	2	116	20	90	526	586
223	411	421	20	90	862	917
224	315	260	10	90	223	278
225	136	288	10	90	1023	1063
226	173	490	20	90	406	462
227	90	232	10	90	682	732
228	402	265	20	90	306	365
229	361	14	10	90	686	738
230	348	147	10	90	845	914
231	385	295	20	90	142	182
232	394	23	20	90	1271	1326
233	466	295	20	90	1033	1073
234	71	103	20	90	1402	1472
235	125	29	30	90	588	649
236	25	65	10	90	1005	1057
237	58	449	30	90	993	1042
238	246	398	40	90	148	199
239	423	294	10	90	790	863
240	59	459	20	90	622	670
241	348	145	20	90	763	813
242	477	97	10	90	421	495
243	307	102	40	90	602	647
244	169	31	20	90	387	447
245	381	495	20	90	705	769
246	95	234	10	90	315	362
247	22	33	10	90	735	803
248	435	268	10	90	1100	1154
249	135	293	30	90	360	430
250	439	266	10	90	267	293
251	269	402	20	90	587	648
252	267	400	10	90	779	836
253	473	196	10	90	564	642
254	445	237	10	90	349	402
255	461	284	10	90	275	340
256	167	339	10	90	648	710
257	26	43	10	90	365	427
258	101	386	20	90	349	415
259	342	54	20	90	553	616
260	243	399	10	90	782	837
261	288	44	20	90	636	711
262	274	48	10	90	368	405
263	360	184	10	90	846	906
264	23	67	30	90	1092	1157
265	6	296	10	90	863	926
266	249	258	30	90	910	974
267	421	415	30	90	297	364
268	319	468	10	90	289	353
269	50	438	20	90	724	779
270	440	61	30	90	971	1032
271	17	65	20	90	625	699
272	323	118	20	90	304	367
273	402	281	20	90	886	924
274	71	153	20	90	746	816
275	52	432	20	90	789	843
276	378	199	10	90	761	804
277	238	204	20	90	566	634
278	208	184	20	90	146	192
279	376	195	10	90	576	616
280	473	475	10	90	1124	1181
281	215	395	20	90	865	928
282	123	222	30	90	376	430
283	436	241	20	90	1271	1333
284	70	100	10	90	1119	1191
285	73	328	20	90	193	270
286	241	205	30	90	669	717
287	250	405	20	90	303	370
288	483	14	10	90	664	732
289	466	287	10	90	467	525
290	467	114	10	90	1159	1223
291	120	159	10	90	306	377
292	408	84	10	90	745	814
293	413	217	20	90	230	289
294	338	142	40	90	204	256
295	483	457	20	90	735	811
296	479	169	20	90	774	834
297	95	224	20	90	1057	1120
298	122	216	10	90	461	537
299	477	179	20	90	237	308
300	406	99	20	90	367	438
301	44	47	20	90	289	356
302	487	450	10	90	929	993
303	172	488	10	90	310	374
304	460	374	20	90	753	831
305	279	138	40	90	281	320
306	338	147	10	90	135	193
307	395	295	20	90	677	736
308	195	185	20	90	335	399
309	463	194	20	90	220	279
310	30	72	10	90	1379	1434
311	244	254	20	90	723	787
312	317	261	20	90	398	473
313	67	108	10	90	297	346
314	442	310	20	90	643	698
315	63	336	20	90	822	861
316	109	107	20	90	357	416
317	100	387	10	90	449	498
318	494	94	10	90	634	692
319	343	49	10	90	650	710
320	40	52	40	90	546	591
321	319	124	20	90	400	465
322	94	97	20	90	285	334
323	476	94	20	90	334	396
324	283	47	10	90	373	420
325	398	325	30	90	774	843
326	460	13	20	90	462	532
327	305	107	20	90	688	753
328	188	12	30	90	245	309
329	136	290	20	90	272	332
330	439	268	10	90	1005	1060
331	112	467	10	90	505	557
332	181	469	20	90	468	536
333	358	18	10	90	485	568
334	459	10	10	90	566	614
335	199	268	20	90	284	368
336	436	237	20	90	186	264
337	318	280	20	90	319	384
338	64	99	10	90	853	904
339	394	333	20	90	492	567
340	366	93	20	90	936	989
341	95	391	10	90	909	973
342	391	425	20	90	842	909
343	344	146	10	90	496	529
344	198	12	10	90	582	631
345	186	472	20	90	564	633
346	390	408	30	90	450	520
347	19	43	20	90	1203	1259
348	307	108	10	90	777	847
349	59	457	30	90	810	848
350	49	451	20	90	340	409
351	125	39	20	90	245	318
352	61	147	40	90	383	413
353	317	123	20	90	497	554
354	392	426	10	90	666	722
355	237	213	10	90	846	917
356	71	333	10	90	252	325
357	304	97	20	90	407	466
358	196	462	10	90	955	1003
359	224	191	20	90	312	359
360	37	51	10	90	633	689
361	491	25	10	90	957	1010
362	270	401	20	90	217	268
363	474	461	30	90	307	379
364	66	448	10	90	270	345
365	337	241	20	90	334	403
366	269	46	20	90	733	794
367	22	41	10	90	1110	1164
368	198	271	20	90	380	458
369	287	43	10	90	555	609
370	446	63	20	90	697	750
371	200	229	30	90	301	355
372	48	72	30	90	711	768
373	465	142	10	90	401	445
374	123	155	10	90	1143	1209
375	16	497	20	90	1038	1123
376	220	319	30	90	971	1037
377	484	171	10	90	680	736
378	55	431	20	90	694	752
379	428	24	10	90	1105	1140
380	80	342	10	90	193	275
381	16	68	20	90	521	617
382	124	56	20	90	231	285
383	95	218	20	90	873	927
384	481	26	30	90	321	374
385	145	434	20	90	462	514
386	335	57	30	90	210	261
387	26	490	10	90	1345	1394
388	324	127	30	90	143	200
389	109	114	10	90	829	871
390	72	148	20	90	265	330
391	220	392	20	90	970	1015
392	442	243	10	90	996	1052
393	69	336	20	90	440	507
394	131	226	20	90	121	183
395	150	204	20	90	639	694
396	423	217	20	90	785	849
397	419	459	20	90	785	838
398	285	163	10	90	714	780
399	203	7	20	90	1033	1108
400	54	445	10	90	534	593
401	109	463	30	90	959	1018
402	355	180	30	90	279	345
403	13	455	10	90	381	430
404	165	337	20	90	373	422
405	314	124	20	90	775	836
406	9	124	20	90	271	336
407	3	292	20	90	404	457
408	16	462	10	90	935	986
409	189	464	10	90	856	906
410	288	154	10	90	442	494
411	90	296	20	90	166	240
412	138	438	20	90	832	893
413	13	459	20	90	646	718
414	461	8	20	90	756	812
415	96	97	20	90	217	280
416	68	99	10	90	1040	1085
417	146	205	10	90	363	417
418	117	152	20	90	859	935
419	481	96	20	90	526	577
420	338	260	20	90	622	678
421	285	44	10	90	459	522
422	111	464	10	90	775	837
423	468	291	30	90	748	798
424	148	206	20	90	458	506
425	184	464	10	90	285	344
426	197	11	10	90	678	719
427	396	410	10	90	829	880
428	133	53	20	90	668	720
429	132	30	20	90	875	926
430	464	13	20	90	853	906
431	100	394	10	90	1006	1067
432	483	22	10	90	476	543
433	93	235	10	90	951	1012
434	174	26	10	90	1037	1105
435	436	264	20	90	186	248
436	24	492	10	90	1252	1302
437	406	94	10	90	375	430
438	234	246	10	90	537	582
439	315	287	30	90	693	754
440	408	453	10	90	575	661
441	128	225	10	90	176	253
442	226	191	20	90	397	459
443	391	407	10	90	549	605
444	198	264	20	90	770	821
445	179	7	20	90	864	930
446	425	215	10	90	699	748
447	483	27	20	90	1322	1384
448	22	40	30	90	1012	1080
449	479	176	20	90	296	367
450	476	102	20	90	797	866
451	386	405	20	90	206	266
452	22	38	20	90	929	980
453	440	241	10	90	1088	1145
454	182	14	10	90	1062	1116
455	266	405	20	90	680	745
456	357	181	20	90	367	441
457	395	332	10	90	593	648
458	423	290	10	90	329	389
459	104	28	20	90	716	748
460	131	56	10	90	489	533
461	111	110	20	90	550	590
462	97	93	20	90	460	530
463	90	274	20	90	411	458
464	214	394	10	90	778	834
465	6	114	10	90	809	855
466	375	191	30	90	473	531
467	356	21	20	90	414	452
468	466	165	30	90	747	805
469	415	223	20	90	1257	1302
470	397	20	20	90	433	477
471	241	403	10	90	686	744
472	69	334	30	90	348	415
473	152	210	20	90	105	163
474	67	335	20	90	629	684
475	126	30	10	90	495	558
476	390	423	10	90	290	336
477	285	162	30	90	625	688
478	285	158	30	90	340	405
479	406	96	20	90	280	342
480	459	374	20	90	848	917
481	67	109	20	90	388	438
482	468	475	10	90	313	373
483	438	31	10	90	348	413
484	168	24	10	90	770	816
485	267	394	10	90	962	1027
486	406	450	20	90	405	464
487	460	288	10	90	213	265
488	224	203	20	90	53	90
489	21	487	10	90	582	629
490	21	488	10	90	667	725
491	87	217	10	90	597	644
492	67	452	30	90	327	402
493	119	223	10	90	853	906
494	398	428	10	90	561	634
495	344	262	20	90	522	585
496	390	301	10	90	391	450
497	167	27	30	90	487	536
498	236	247	20	90	443	492
499	313	119	30	90	679	742
500	13	51	10	90	1366	1425
501	200	13	10	90	397	453
502	0	120	30	90	431	492
503	202	4	10	90	950	1004
504	343	144	10	90	302	349
505	94	383	10	90	636	687
506	250	411	30	90	492	554
507	464	134	20	90	1048	1106
508	443	237	20	90	248	318
509	4	114	10	90	712	769
510	169	336	30	90	838	892
511	200	178	20	90	240	298
512	412	453	10	90	674	750
513	121	151	20	90	1053	1110
514	403	93	10	90	1131	1181
515	399	301	30	90	584	634
516	97	387	10	90	823	869
517	176	8	10	90	969	1012
518	62	326	20	90	914	969
519	151	207	10	90	731	788
520	299	98	30	90	320	364
521	334	240	30	90	526	582
522	92	270	20	90	954	1029
523	444	269	20	90	544	593
524	132	223	30	90	837	909
525	428	25	20	90	1175	1251
526	413	421	10	90	768	828
527	382	498	10	90	619	669
528	391	429	20	90	475	525
529	398	97	10	90	264	344
530	192	265	10	90	678	721
531	438	26	10	90	453	497
532	44	79	10	90	267	333
533	137	290	10	90	176	245
534	446	319	30	90	364	416
535	175	487	20	90	248	308
536	26	45	30	90	303	366
537	147	435	20	90	553	607
538	111	459	20	90	1152	1195
539	472	132	10	90	767	826
540	349	158	20	90	1050	1093
541	86	268	10	90	689	735
542	415	216	10	90	319	384
543	121	218	20	90	191	257
544	210	185	20	90	76	120
545	98	215	20	90	218	278
546	484	174	10	90	496	544
547	328	458	10	90	944	1006
548	313	126	10	90	873	923
549	390	415	20	90	1042	1090
550	459	17	20	90	1039	1094
551	481	124	10	90	505	571
552	386	414	10	90	1109	1162
553	116	466	20	90	318	373
554	468	295	10	90	934	987
555	391	426	10	90	752	818
556	408	279	20	90	682	747
557	15	68	10	90	448	508
558	392	424	10	90	926	1007
559	88	95	20	90	658	712
560	116	151	20	90	774	836
561	240	250	10	90	340	405
562	123	159	20	90	156	216
563	89	290	20	90	225	300
564	39	50	30	90	440	511
565	359	179	20	90	465	529
566	338	257	20	90	713	772
567	92	236	20	90	1041	1104
568	413	417	10	90	1135	1207
569	91	101	10	90	844	902
570	55	449	20	90	278	339
571	57	425	20	90	260	317
572	465	287	10	90	556	617
573	320	280	10	90	221	298
574	322	468	20	90	575	625
575	483	458	10	90	656	708
576	312	267	10	90	862	941
577	139	291	10	90	118	187
578	388	408	10	90	361	425
579	110	459	30	90	1057	1109
580	459	372	20	90	672	726
581	204	269	30	90	49	98
582	407	98	10	90	218	297
583	335	256	10	90	890	962
584	471	484	10	90	571	618
585	339	244	30	90	251	299
586	89	272	10	90	498	555
587	488	96	10	90	909	973
588	316	286	30	90	601	662
589	235	50	30	90	813	896
590	316	258	20	90	317	368
591	425	288	10	90	603	669
592	213	397	10	90	680	744
593	113	156	10	90	577	661
594	199	9	20	90	853	911
595	463	167	10	90	657	709
596	129	27	10	90	680	745
597	248	404	30	90	214	274
598	405	276	20	90	771	848
599	400	288	30	90	154	232
600	61	156	30	90	462	532
601	393	20	20	90	1190	1222
602	484	177	20	90	390	463
603	108	116	30	90	911	973
604	112	217	20	90	383	443
605	389	404	10	90	267	331
606	102	391	20	90	1098	1161
607	434	26	10	90	538	601
608	488	26	30	90	1136	1199
609	477	120	10	90	968	1026
610	126	220	20	90	659	711
611	439	310	10	90	198	256
612	212	319	30	90	227	293
613	320	283	30	90	421	471
614	17	459	10	90	1200	1275
615	236	213	20	90	39	114
616	68	108	10	90	230	324
617	390	294	20	90	853	931
618	275	42	30	90	537	608
619	423	214	20	90	595	669
620	460	367	20	90	392	450
621	272	402	10	90	317	353
622	478	102	10	90	704	775
623	400	260	40	90	150	212
624	181	6	20	90	678	753
625	470	475	30	90	381	429
626	245	408	10	90	585	652
627	93	96	20	90	374	427
628	218	318	20	90	884	939
629	441	265	40	90	344	400
630	21	64	20	90	818	876
631	356	23	20	90	309	373
632	267	44	20	90	649	693
633	277	50	10	90	257	329
634	182	4	10	90	586	659
635	26	36	10	90	646	702
636	384	491	20	90	897	948
637	418	290	10	90	1172	1221
638	321	277	10	90	75	140
639	358	16	30	90	596	641
640	389	334	20	90	316	373
641	48	78	20	90	990	1042
642	24	68	20	90	1282	1337
643	206	225	10	90	405	446
644	290	141	30	90	645	709
645	439	15	20	90	650	706
646	359	182	20	90	748	821
647	413	287	10	90	1348	1417
648	230	49	20	90	201	253
649	407	280	10	90	595	653
650	459	295	20	90	1209	1273
651	468	199	20	90	669	730
652	101	384	30	90	200	262
653	170	26	10	90	855	918
654	197	270	30	90	474	548
655	415	289	20	90	169	228
656	203	390	40	90	307	361
657	443	272	10	90	821	869
658	210	391	30	90	202	272
659	363	22	20	90	964	1025
660	352	176	20	90	126	175
661	212	223	40	90	758	832
662	8	120	10	90	1169	1232
663	244	250	10	90	157	217
664	18	484	10	90	397	444
665	379	82	10	90	827	884
666	84	346	20	90	1095	1157
667	233	207	10	90	105	167
668	344	62	10	90	1208	1274
669	20	489	10	90	856	901
670	361	181	20	90	662	723
671	174	494	10	90	498	559
672	49	76	20	90	899	950
673	390	325	30	90	883	929
674	7	118	20	90	988	1047
675	107	26	10	90	517	579
676	312	270	30	90	967	1022
677	399	102	20	90	649	715
678	109	108	10	90	444	510
679	115	218	10	90	298	341
680	88	287	30	90	878	959
681	54	424	10	90	320	387
682	125	32	30	90	407	462
683	199	12	20	90	475	557
684	457	374	10	90	945	1005
685	391	413	10	90	1013	1069
686	304	102	30	90	505	559
687	391	293	20	90	768	834
688	66	336	10	90	719	778
689	132	291	10	90	736	797
690	18	462	20	90	1015	1090
691	347	62	10	90	1121	1176
692	466	142	10	90	492	537
693	389	17	10	90	993	1049
694	342	254	10	90	244	301
695	92	223	10	90	967	1023
696	404	83	30	90	831	916
697	290	46	20	90	832	896
698	95	92	20	90	554	621
699	200	265	10	90	853	922
700	435	16	10	90	829	895
701	25	499	10	90	1154	1206
702	319	260	20	90	489	566
703	116	225	20	90	752	820
704	402	284	30	90	313	367
705	108	105	20	90	258	329
706	386	13	10	90	785	878
707	472	129	10	90	312	383
708	132	57	20	90	396	443
709	43	71	40	90	525	581
710	493	91	30	90	542	599
711	12	451	20	90	311	359
712	201	188	10	90	526	581
713	60	428	20	90	1059	1137
714	243	248	30	90	243	314
715	230	197	40	90	588	645
716	389	11	20	90	707	769
717	401	281	20	90	968	1024
718	459	14	20	90	372	439
719	411	95	30	90	475	521
720	0	297	10	90	679	737
721	476	196	30	90	476	544
722	458	16	20	90	313	369
723	92	230	20	90	498	549
724	432	24	20	90	1000	1056
725	215	317	10	90	140	194
726	371	192	30	90	190	259
727	481	20	20	90	572	631
728	152	207	20	90	824	877
729	14	459	20	90	559	622
730	361	17	20	90	777	832
731	8	497	40	90	959	1006
732	206	186	10	90	891	955
733	421	218	10	90	880	939
734	402	264	20	90	216	273
735	388	325	10	90	971	1026
736	35	304	30	90	280	345
737	321	263	20	90	680	743
738	444	313	20	90	462	511
739	188	462	40	90	753	826
740	400	103	10	90	546	636
741	418	221	10	90	1150	1223
742	112	219	20	90	469	541
743	408	452	10	90	490	564
744	30	301	10	90	660	726
745	205	228	10	90	110	173
746	407	88	10	90	1035	1083
747	380	489	20	90	1084	1143
748	395	331	20	90	690	734
749	133	26	10	90	776	837
750	21	68	20	90	1189	1243
751	313	282	20	90	790	847
752	53	440	10	90	633	684
753	458	130	20	90	1140	1210
754	437	15	30	90	739	801
755	400	286	10	90	217	276
756	346	144	10	90	661	729
757	440	244	10	90	809	874
758	280	162	30	90	158	210
759	488	173	20	90	583	645
760	326	118	10	90	213	272
761	207	400	20	90	489	562
762	391	18	10	90	1085	1141
763	37	55	10	90	730	780
764	204	15	20	90	239	301
765	173	28	10	90	1134	1191
766	343	148	20	90	399	441
767	231	43	10	90	447	513
768	445	273	20	90	725	781
769	432	27	10	90	810	878
770	439	305	30	90	736	797
771	208	396	20	90	401	462
772	249	407	10	90	403	455
773	107	112	30	90	728	787
774	279	159	20	90	252	301
775	376	494	30	90	334	396
776	403	262	30	90	493	547
777	41	47	20	90	359	406
778	96	213	30	90	311	369
779	461	167	10	90	470	531
780	391	99	20	90	206	268
781	116	221	30	90	571	627
782	165	343	10	90	556	613
783	388	334	20	90	227	280
784	130	57	40	90	289	365
785	422	409	10	90	234	290
786	66	156	10	90	653	718
787	485	86	20	90	349	413
788	389	428	20	90	380	435
789	117	467	10	90	254	307
790	475	477	20	90	1028	1091
791	429	27	20	90	1272	1339
792	339	60	20	90	274	338
793	381	494	10	90	801	855
794	218	321	20	90	697	759
795	64	101	20	90	667	724
796	343	254	20	90	334	394
797	478	464	10	90	360	446
798	423	286	20	90	430	477
799	198	9	20	90	763	819
800	225	196	10	90	128	174
801	374	190	20	90	292	344
802	462	14	20	90	944	1000
803	347	54	10	90	836	904
804	231	195	20	90	497	552
805	72	96	20	90	1220	1278
806	489	87	20	90	451	499
807	443	313	30	90	554	602
808	233	204	20	90	198	260
809	477	483	20	90	745	817
810	459	368	10	90	299	361
811	95	277	20	90	1057	1121
812	16	460	20	90	1112	1180
813	100	266	20	90	207	275
814	288	53	40	90	925	997
815	211	386	20	90	141	212
816	144	439	30	90	736	797
817	224	314	10	90	1074	1126
818	79	291	10	90	519	568
819	95	235	10	90	155	227
820	7	123	30	90	329	399
821	91	217	10	90	692	738
822	105	27	10	90	611	670
823	101	385	20	90	262	320
824	238	210	30	90	757	821
825	439	67	10	90	322	386
826	435	267	20	90	1188	1247
827	443	64	30	90	779	854
828	92	234	10	90	853	926
829	450	265	10	90	444	498
830	102	27	20	90	796	852
831	492	94	10	90	724	787
832	440	63	20	90	886	934
833	453	367	20	90	1055	1091
834	15	69	10	90	359	414
835	489	17	10	90	764	826
836	335	255	20	90	85	124
837	419	418	10	90	394	454
838	292	41	10	90	733	804
839	489	27	10	90	1035	1117
840	96	270	20	90	308	367
841	461	369	10	90	579	633
842	105	22	30	90	339	385
843	479	121	10	90	692	754
844	207	223	10	90	585	634
845	17	458	10	90	1297	1360
846	186	7	20	90	500	555
847	465	168	10	90	837	902
848	416	288	20	90	1251	1328
849	388	418	10	90	217	283
850	92	233	10	90	768	830
851	179	492	10	90	678	752
852	132	55	10	90	573	632
853	485	24	20	90	387	445
854	388	331	20	90	160	230
855	217	323	20	90	424	473
856	281	164	20	90	91	140
857	169	340	10	90	737	806
858	491	20	20	90	853	925
859	279	42	40	90	262	338
860	439	243	20	90	1191	1227
861	391	334	20	90	402	470
862	96	244	10	90	1332	1380
863	441	273	20	90	903	972
864	477	478	20	90	942	993
865	56	434	20	90	881	940
866	12	463	20	90	751	800
867	458	164	20	90	374	439
868	446	65	20	90	516	566
869	71	156	10	90	844	904
870	65	153	30	90	570	614
871	8	118	20	90	1079	1138
872	102	29	30	90	887	946
873	315	265	10	90	774	841
874	343	62	10	90	1300	1364
875	384	16	20	90	892	959
876	67	105	10	90	579	622
877	478	118	20	90	1058	1121
878	73	101	20	90	1313	1375
879	200	14	10	90	311	356
880	114	225	30	90	664	723
881	460	293	30	90	1121	1176
882	51	447	10	90	438	501
883	418	295	20	90	1063	1140
884	282	139	20	90	188	227
885	477	122	10	90	886	924
886	91	266	10	90	776	840
887	16	463	20	90	836	904
888	228	199	10	90	673	746
889	435	20	20	90	933	980
890	467	195	10	90	285	344
891	469	132	10	90	856	923
892	465	136	30	90	964	1007
893	246	255	10	90	818	877
894	49	75	20	90	810	857
895	66	98	20	90	944	996
896	166	32	10	90	296	352
897	326	466	30	90	753	817
898	202	186	20	90	616	676
899	467	285	20	90	380	428
900	290	136	20	90	549	614
901	341	58	10	90	364	433
902	121	219	20	90	132	203
903	107	35	30	90	986	1041
904	216	319	10	90	321	387
905	134	288	20	90	923	979
906	89	270	10	90	590	647
907	136	52	10	90	759	817
908	462	167	20	90	566	617
909	340	54	10	90	461	525
910	80	286	10	90	601	677
911	38	301	20	90	768	814
912	25	307	10	90	480	529
913	470	139	10	90	668	731
914	270	400	10	90	151	213
915	204	187	30	90	803	857
916	460	171	30	90	937	994
917	399	106	20	90	925	989
918	197	267	10	90	582	626
919	77	350	10	90	997	1058
920	91	231	30	90	579	652
921	398	103	30	90	739	807
922	35	303	20	90	221	262
923	231	204	20	90	776	834
924	311	266	10	90	63	126
925	416	420	20	90	663	746
926	231	203	20	90	291	352
927	4	116	10	90	618	678
928	417	217	30	90	423	465
929	268	52	10	90	1003	1079
930	345	144	30	90	568	640
931	368	85	30	90	202	275
932	441	65	10	90	422	470
933	201	270	10	90	108	177
934	0	293	30	90	591	637
935	167	24	20	90	668	736
936	186	464	10	90	662	730
937	7	292	10	90	246	306
938	80	290	10	90	420	484
939	405	455	40	90	883	948
940	439	312	30	90	257	324
941	300	108	10	90	150	211
942	478	121	40	90	788	840
943	92	232	10	90	401	462
944	111	463	20	90	875	919
945	132	294	20	90	454	521
946	134	285	20	90	1104	1168
947	316	284	20	90	510	570
948	56	428	20	90	415	482
949	232	47	10	90	262	327
950	466	291	10	90	656	705
951	341	257	20	90	424	490
952	420	213	10	90	509	568
953	116	152	20	90	678	750
954	163	337	10	90	277	334
955	440	7	20	90	469	508
956	208	222	30	90	667	734
957	485	95	10	90	281	351
958	97	385	30	90	537	598
959	462	367	20	90	481	545
960	455	365	20	90	235	306
961	492	95	30	90	815	877
962	405	450	20	90	311	376
963	371	193	20	90	133	185
964	234	202	10	90	389	440
965	404	103	20	90	464	530
966	24	41	10	90	557	601
967	67	334	20	90	532	600
968	207	227	10	90	48	112
969	277	403	10	90	395	464
970	15	457	20	90	462	534
971	217	319	10	90	789	851
972	111	467	20	90	597	647
973	378	493	30	90	426	487
974	112	465	10	90	685	743
975	173	25	20	90	952	1007
976	421	292	20	90	245	287
977	74	146	30	90	204	259
978	178	491	10	90	591	656
979	204	185	20	90	707	769
980	319	470	10	90	383	442
981	163	331	40	90	118	179
982	470	125	20	90	253	316
983	315	121	10	90	590	647
984	166	34	20	90	231	305
985	284	140	40	90	115	172
986	85	288	20	90	329	386
987	376	190	30	90	382	439
988	238	203	10	90	483	534
989	399	104	10	90	834	896
990	472	481	20	90	477	527
991	46	74	20	90	612	682
992	401	255	30	90	685	738
993	475	480	30	90	837	911
994	485	27	10	90	1231	1291
995	476	174	30	90	874	924
996	330	242	30	90	627	671
997	332	249	30	90	82	144
998	375	80	10	90	550	598
999	94	235	20	90	227	266
1000	287	144	20	90	827	895
\.


DROP TABLE IF EXISTS homberger_1000_c1_10_1.vehicles;
CREATE TABLE IF NOT EXISTS homberger_1000_c1_10_1.vehicles (
  id            BIGINT    PRIMARY KEY,
  start_index   BIGINT    NOT NULL,
  end_index     BIGINT    NOT NULL,
  capacity      BIGINT    NOT NULL,
  tw_open       INTEGER   NOT NULL,
  tw_close      INTEGER   NOT NULL
);

INSERT INTO homberger_1000_c1_10_1.vehicles (id, start_index, end_index, capacity, tw_open, tw_close)
VALUES(generate_series(1, 250), 2500250, 2500250, 200, 0, 1824);


DROP TABLE IF EXISTS homberger_1000_c1_10_1.matrix;
CREATE TABLE IF NOT EXISTS homberger_1000_c1_10_1.matrix (
  start_id   BIGINT    NOT NULL,
  end_id     BIGINT    NOT NULL,
  duration   INTEGER   NOT NULL,
  PRIMARY KEY (start_id, end_id)
);

WITH
the_matrix_info AS (
    SELECT
      A.location_index AS start_id,
      B.location_index AS end_id,
      ROUND(sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y)))::INTEGER AS duration
    FROM jobs AS A, jobs AS B WHERE A.id != B.id
)
INSERT INTO homberger_1000_c1_10_1.matrix
SELECT start_id, end_id, duration
FROM the_matrix_info;


/*
-- To run the query (takes around 01:05 minutes)
SELECT * FROM vrp_vroomJobsPlain(
  'SELECT id, location_index, ARRAY[delivery], service FROM jobs WHERE id > 0',
  'SELECT id, tw_open, tw_close FROM jobs WHERE id > 0',
  'SELECT id, start_index, end_index, ARRAY[capacity], tw_open, tw_close FROM vehicles',
  NULL,
  NULL,
  'SELECT * FROM matrix'
);
*/

/*
-- Modification for non-plain functions
ALTER TABLE jobs ALTER COLUMN service TYPE INTERVAL USING make_interval(secs => service);
ALTER TABLE jobs ALTER COLUMN tw_open TYPE TIMESTAMP USING (to_timestamp(tw_open + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE jobs ALTER COLUMN tw_close TYPE TIMESTAMP USING (to_timestamp(tw_close + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vehicles ALTER COLUMN tw_open TYPE TIMESTAMP USING (to_timestamp(tw_open + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vehicles ALTER COLUMN tw_close TYPE TIMESTAMP USING (to_timestamp(tw_close + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE matrix ALTER COLUMN duration TYPE INTERVAL USING make_interval(secs => duration);
*/
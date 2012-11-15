/****************************************************************************
**
** Copyright (C) 2011 Andrey Kartashov .
** All rights reserved.
** Contact: Andrey Kartashov (porter@porter.st)
**
** This file is part of the ReadsCounting module of the genome-tools.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Andrey Kartashov.
**
****************************************************************************/
#include <Threads.hpp>
#include <SamReader.hpp>


void sam_reader_thread::run(void)
{
    qDebug()<<fileName<<"- started";
    SamReader<gen_lines> (fileName,sam_data).Load();
    qDebug()<<fileName<<"- bam loaded";

    foreach(const QString key,isoforms[0].keys())/*Iterating trough chromosomes*/
        for(int i=0; i< isoforms[0][key].size();i++)/*Iterating trough isoforms on chromosomes*/
        {
            if(isoforms[0][key][i]->testNeeded) continue;
            if(isoforms[0][key][i]->intersects)
            {

                chrom_coverage::iterator it_count = isoforms[0][key][i]->intersects_count->begin();

                for(; it_count != isoforms[0][key][i]->intersects_count->end(); it_count++)
                {
                    chrom_coverage::interval_type itv  =
                            bicl::key_value<chrom_coverage >(it_count);
                    quint64 tot=0;

                    /**/

                    chrom_coverage::domain_type l=itv.lower();
                    chrom_coverage::domain_type u=itv.upper();

                    if(itv.bounds().bits() == bicl::interval_bounds::_left_open)
                    {
                        l++;
                    }
                    else if(itv.bounds().bits() == bicl::interval_bounds::_right_open)
                    {
                        u--;
                    }
                    else if(itv.bounds().bits() == bicl::interval_bounds::_open)
                    {
                        l++;
                        u--;
                    }
                    tot+=sam_data->getLineCover(isoforms[0][key][i]->chrom+QChar('+')).getStarts(l,u);
                    tot+=sam_data->getLineCover(isoforms[0][key][i]->chrom+QChar('-')).getStarts(l,u);


                    /*Calculating densities */
                    for(int c=0;c<isoforms[0][key][i]->intersects_isoforms->size();c++)
                    {
                        if(bicl::intersects(isoforms[0][key][i]->intersects_isoforms->at(c)->isoform,itv))
                        {
                            /*DEBUG*/
                            if(isoforms[0][key][i]->name2.startsWith("RPS3"))
                            {
                                qDebug()<<"name:"<<isoforms[0][key][i]->name<<
                                          " name2:"<<isoforms[0][key][i]->name2<<"totlen:"<<isoforms[0][key][i]->intersects_isoforms->at(c)->isoform.size()<<" c:"<<c+1<<" al: 1"<<" segment:["<<l<<":"<<u<<"] len:"<<(u-l+1)<<" reads: "<<tot<<" density:"<<(float)tot/(u-l+1);

                            }
                            /*DEBUG*/

                            isoforms[0][key][i]->intersects_isoforms->at(c)->totReads+=tot;
                            if(isoforms[0][key][i]->intersects_isoforms->at(c)->min > it_count->second)
                            {
                                isoforms[0][key][i]->intersects_isoforms->at(c)->min=it_count->second;

                                isoforms[0][key][i]->intersects_isoforms->at(c)->count=1;
                                isoforms[0][key][i]->intersects_isoforms->at(c)->density=((float)tot/(u-l+1))/it_count->second;
                            }
                            else if(isoforms[0][key][i]->intersects_isoforms->at(c)->min==it_count->second)
                            {
                                isoforms[0][key][i]->intersects_isoforms->at(c)->count++;
                                isoforms[0][key][i]->intersects_isoforms->at(c)->density+=((float)tot/(u-l+1))/it_count->second;
                            }
                        }
                        else
                        {
                            /*DEBUG*/
                            if(isoforms[0][key][i]->name2.startsWith("RPS3"))
                            {
                                qDebug()<<"name:"<<isoforms[0][key][i]->name<<
                                          " name2:"<<isoforms[0][key][i]->name2<<"totlen:"<<isoforms[0][key][i]->intersects_isoforms->at(c)->isoform.size()<<" c:"<<c+1<<" al: 0"<<" segment:["<<l<<":"<<u<<"] len:"<<(u-l+1);

                            }
                            /*DEBUG*/
                        }
                    }
                }

                /*calculate total density*/
                float tot_density=0.0;
                for(int c=0;c<isoforms[0][key][i]->intersects_isoforms->size();c++)
                {
                    isoforms[0][key][i]->intersects_isoforms->at(c)->testNeeded=true;
                    isoforms[0][key][i]->intersects_isoforms->at(c)->density/=isoforms[0][key][i]->intersects_isoforms->at(c)->count;
                    tot_density+=isoforms[0][key][i]->intersects_isoforms->at(c)->density;
                }
                if(tot_density<0.000000001) tot_density=1.0;

                for(int c=0;c<isoforms[0][key][i]->intersects_isoforms->size();c++)
                {
                    float pk=(float)isoforms[0][key][i]->intersects_isoforms->at(c)->isoform.size()/1000.0;
                    float pm=(float)(sam_data->total-sam_data->notAligned)/1000000.0;
                    float co=isoforms[0][key][i]->intersects_isoforms->at(c)->density/tot_density;
                    isoforms[0][key][i]->intersects_isoforms->at(c)->RPKM=
                            isoforms[0][key][i]->intersects_isoforms->at(c)->totReads*co/(pk*pm);

                    if(isoforms[0][key][i]->intersects_isoforms->at(c)->name2.startsWith("RPS3"))
                    {
                        //<<" size:"<<reads.size()
                        qDebug()<<"name:"<<isoforms[0][key][i]->intersects_isoforms->at(c)->name<<
                                  " name2:"<<isoforms[0][key][i]->intersects_isoforms->at(c)->name2 <<" tot_density:" << tot_density << " min:"<<
                                  isoforms[0][key][i]->intersects_isoforms->at(c)->min << " density:"<<
                                  isoforms[0][key][i]->intersects_isoforms->at(c)->density << " size:"<<isoforms[0][key][i]->intersects_isoforms->at(c)->isoform.size()<<
                                  " pk:"<<pk<<" pm:"<<pm << " rpkm:"<<isoforms[0][key][i]->intersects_isoforms->at(c)->RPKM;
                    }
                }
            }
            /*if not intersects*/
            else
            {
                chrom_coverage::iterator it = isoforms[0][key][i]->isoform.begin();
                quint64 tot=0;
                for(; it != isoforms[0][key][i]->isoform.end(); it++)
                {
                    chrom_coverage::interval_type itv  =
                            bicl::key_value<chrom_coverage >(it);

                    tot+=sam_data->getLineCover(isoforms[0][key][i]->chrom+QChar('+')).getStarts(itv.lower(),itv.upper());
                    tot+=sam_data->getLineCover(isoforms[0][key][i]->chrom+QChar('-')).getStarts(itv.lower(),itv.upper());
                }
                isoforms[0][key][i]->totReads=tot;
                isoforms[0][key][i]->RPKM=
                        ((float)(tot)/((float)(isoforms[0][key][i]->isoform.size())/1000.0))/((float)(sam_data->total-sam_data->notAligned)/1000000.0);
            }
        }
    qDebug()<<fileName<<"- finished";
    QTimer::singleShot(10, this, SLOT(quit()));
    this->exec();
}

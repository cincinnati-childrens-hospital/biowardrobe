#include "atdpbasics.h"

ATDPBasics::ATDPBasics(EXPERIMENT_INFO* e)
{
    exp_i=e;
    avd_window=gArgs().getArgs("avd_window").toInt();
    avd_whole_region=avd_window*2+1;
    avd_heat_window=gArgs().getArgs("avd_heat_window").toInt();
    twicechr=gArgs().getArgs("sam_twicechr").toString();
    ignorechr=gArgs().getArgs("sam_ignorechr").toString();


    /* Open bam files preparing EXPERIMENT_INFO class
     */
    QString fp=gSettings().getValue("wardrobe")+"/"+gSettings().getValue("preliminary")+"/"+exp_i->filepath;
    //    if ( !exp_i->reader.Open(exp_i->filepath.toStdString()) ) {
    if ( !exp_i->reader.Open(fp.toStdString()) ) {
        qDebug() << "Could not open input BAM files.";
        throw "Could not open input BAM files";
    }

    exp_i->header = exp_i->reader.GetHeader();
    exp_i->references = exp_i->reader.GetReferenceData();

    /* Organazing chrom ids
     */
    for(int RefID=0;RefID < (int)exp_i->references.size();RefID++) {
        if(ignorechr.contains(exp_i->references[RefID].RefName.c_str())) {
            exp_i->i_tids<<RefID;
            continue;
        }
        if(twicechr.contains(exp_i->references[RefID].RefName.c_str())) {
            exp_i->tids<<RefID;
        }
        exp_i->ref_map.insert(
                    exp_i->references[RefID].RefName.c_str(),
                    QPair<int,int>(RefID,exp_i->references[RefID].RefLength));
    }

    if(!exp_i->reader.LocateIndex()) {
        qDebug() << "Could not locate index.";
        throw "Could not locate index.";
    }
    /*
     * Fill in all regions for the current uid
     */
    this->getRegions();
}

void ATDPBasics::getRegions() {

    QSqlQuery q;

    q.prepare("select * from `"+exp_i->db+"`.`"+exp_i->source+
              "` where chrom not like '%\\_%' and chrom not like '%control%' order by chrom,strand,txStart,txEnd");
    if(!q.exec()) {
        qDebug()<<"Query error info: "<<q.lastError().text();
        throw "Error query to DB";
    }

    QSqlRecord rec=q.record();

    int fieldChrom = rec.indexOf("chrom");
    int fieldStrand = rec.indexOf("strand");
    int fieldTxStart= rec.indexOf("txStart");
    int fieldTxEnd= rec.indexOf("txEnd");
    int fieldRefseq_id= rec.indexOf("name");
    if(fieldRefseq_id==-1)
        fieldRefseq_id= rec.indexOf("refseq_id");
    int fieldGene_id= rec.indexOf("name2");
    if(fieldGene_id==-1)
        fieldGene_id= rec.indexOf("gene_id");

    for(int k=0;k<rec.count();k++) {
        if(rec.fieldName(k).contains("RPKM")) {
            int rpkm_idx=rec.fieldName(k).indexOf("RPKM");
            bool add=true;
            if(rpkm_idx > 0){
                QString orig_name=rec.fieldName(k).mid(rpkm_idx);
                foreach (QJsonValue n, exp_i->rpkmnames) {
                    if(n.toString().contains(orig_name)) {
                        add=false;
                        break;
                    }
                }
            }
            if(add) {
                exp_i->rpkmnames.append(rec.fieldName(k));
            }
        }
    }

    while(q.next()) {
        QString chr=q.value(fieldChrom).toString();
        QChar strand=q.value(fieldStrand).toString().at(0);
        quint64 txStart=q.value(fieldTxStart).toInt();//+1 ??? FIXME
        quint64 txEnd=q.value(fieldTxEnd).toInt();
        QString refseq_id=q.value(fieldRefseq_id).toString();
        QString gene_id=q.value(fieldGene_id).toString();
        if( ignorechr.contains(chr)) {
            continue;
        }

        QSharedPointer<REGION> region(new REGION());

        if(strand=='+'){
            qint64 start=txStart-avd_window-exp_i->fragmentsize/2;
            if(!exp_i->regions.isEmpty() && exp_i->regions.last()->start==start) {
                if(!exp_i->regions.last()->gene_id.contains(gene_id))
                    exp_i->regions.last()->gene_id.append(","+gene_id);
                if(!exp_i->regions.last()->refseq_id.contains(refseq_id))
                    exp_i->regions.last()->refseq_id.append(","+refseq_id);
                region.clear();
                continue;
            }
            region->txStart=txStart;
            region->txEnd=txEnd;
            region->start=start;
            region->end=txStart+avd_window+exp_i->fragmentsize/2;
            region->strand=true;
        } else {
            qint64 start=txEnd-avd_window-exp_i->fragmentsize/2;
            if(!exp_i->regions.isEmpty() && exp_i->regions.last()->start==start) {
                if(!exp_i->regions.last()->gene_id.contains(gene_id))
                    exp_i->regions.last()->gene_id.append(","+gene_id);
                if(!exp_i->regions.last()->refseq_id.contains(refseq_id))
                    exp_i->regions.last()->refseq_id.append(","+refseq_id);
                region.clear();
                continue;
            }
            region->txStart=txStart;
            region->txEnd=txEnd;
            region->start=start;
            region->end=txEnd+avd_window+exp_i->fragmentsize/2;
            region->strand=false;
        }
        region->gene_id=gene_id;
        region->refseq_id=refseq_id;
        region->chrom=chr;
        exp_i->avd_matrix.append(
                    QPair<QSharedPointer<REGION>,QVector<quint16> >(
                        QSharedPointer<REGION>(region),QVector<quint16>(qCeil((qreal)avd_whole_region/avd_heat_window),0) ) );

        if(exp_i->rpkmnames.count()>0) {
            QJsonArray rpkms;
            foreach (QJsonValue n, exp_i->rpkmnames) {
                rpkms.append(q.value(rec.indexOf(n.toString())).toDouble());
            }
            exp_i->rpkm_matrix.append(
                        QPair<QSharedPointer<REGION>,QJsonArray >(
                            QSharedPointer<REGION>(region), rpkms) );
        }
        exp_i->regions.append(region);
    }
}

void ATDPBasics::RegionsProcessing () {
    /*
     *  Work with all regions
     */
    for(int i=0;i<exp_i->regions.size();i++) {
        if(!exp_i->reader.SetRegion(
               exp_i->ref_map[exp_i->regions[i]->chrom].first,
               exp_i->regions[i]->start,
               exp_i->ref_map[exp_i->regions[i]->chrom].first,
               exp_i->regions[i]->end
               )){
            qDebug()<<"Cant set region:"
                   <<exp_i->regions[i]->chrom
                  <<exp_i->regions[i]->start
                 <<exp_i->regions[i]->end;
            throw "Cant set region.";
        } else {
            BamAlignment al;
            int shift=exp_i->fragmentsize/2;
            qint64 left=exp_i->regions[i]->start+shift;

            while ( exp_i->reader.GetNextAlignmentCore(al) ) {

                if(al.IsPaired() && (!al.IsProperPair())) {
                    continue;
                }

                if(al.IsPaired() && al.IsProperPair() && al.IsMateMapped() ) {
                    if( ( (al.Position<al.MatePosition) && al.IsReverseStrand() ) || ( (al.MatePosition < al.Position) && al.IsMateReverseStrand() )) {
                        prn_debug("Name2:",al);
                        continue;
                    }
                }

                int position_b = al.Position+1;
                int position_e = al.GetEndPosition();

                int length=abs(al.InsertSize);

                if(al.IsMateMapped() && al.IsFirstMate() ) { //pair-end reads
                    if( length==0 ) { //bug
                        prn_debug("Name3:",al);
                        continue;
                    }
                    //                        if( (u||l)>0 && (!(l<=length && length<=u)) ) { //fragment length filter, for pair ends
                    //                            output->notAligned+=num;
                    //                            continue;
                    //                        }
                    if(al.IsReverseStrand()) {
                        position_b= position_e-length+1;
                    } else {
                        position_e= position_b+length-1;
                    }
                } else  if(al.IsMateMapped() && al.IsSecondMate()) {
                    continue;
                }

                int b;
                if(al.IsPaired()) {
                    b=position_b-left+length/2;
                } else {
                    if(al.IsReverseStrand()) {// - strand
                        b=position_e-shift-left;
                    } else {
                        b=position_b+shift-left;
                    }
                }
                if(b<0) continue;
                if(b>=avd_whole_region) break;

                quint16 d=1;
                if(exp_i->tids.contains(al.RefID)) {
                    d=2;
                    exp_i->mapped++;
                }
                if(exp_i->regions[i]->strand){
                    exp_i->avd_total[b]+=d;
                    exp_i->avd_matrix[i].second[b/avd_heat_window]+=1;
                } else {
                    exp_i->avd_total[avd_whole_region-b-1]+=d;
                    exp_i->avd_matrix[i].second[(avd_whole_region-b-1)/avd_heat_window]+=1;
                }
            }//trough bam reads while

        }
    }//go trough region

}

/*************
 *
 *
 *   Static functions
 *
 *
 **************/

void ATDPBasics::prn_debug(QString str,BamAlignment &al) {
    qDebug()
            <<"\n"<<str<<       al.Name.c_str()
           <<"\n IsDuplicate:"<<   al.IsDuplicate()
          <<"\n IsFailedQC"<<     al.IsFailedQC()
         <<"\n IsMaped:"<<       al.IsMapped()
        <<"\n isFirstMate:"<<   al.IsFirstMate()
       <<"\n isSecondMate:"<<  al.IsSecondMate()
      <<"\n IsMateMapped:"<<  al.IsMateMapped()
     <<"\n IsMateReverseStrand:"<< al.IsMateReverseStrand() // returns true if alignment's mate mapped to reverse strand
    <<"\n IsReverseStrand:"<<   al.IsReverseStrand()
    <<"\n IsPaired:"<<      al.IsPaired()
    <<"\n IsPrimaryAlignment:"<<al.IsPrimaryAlignment()
    <<"\n IsProperPair:"<<  al.IsProperPair()
    <<"\n AligmentFlag:"<<  al.AlignmentFlag
    <<"\n MateRefId:"<<     al.MateRefID
    <<"\n Len:"<<       al.Length
    <<"\n InsertSize:"<<    al.InsertSize
    <<"\n MatePosition:["<<al.MatePosition<<"] "
    <<"\n Position:["<<al.RefID<<":"<<al.Position+1<<"-"<<al.GetEndPosition()<<"] ";
}


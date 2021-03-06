/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Andrey Kartashov .
 ** All rights reserved.
 ** Contact: Andrey Kartashov (porter@porter.st)
 **
 ** This file is part of the EMS web interface module of the genome-tools.
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

Ext.define("EMS.ux.d3", {
    extend: 'Ext.panel.Panel',
    //    alias: ['widget.d3'],

    resizable: false,

    loadMask: true,
    loadMaskMsg: 'Processing ... ',

    refreshOnChange: false,
    refreshOnLoad: false,
    storeLoaded: false,

    border: false,

    data: null,
    store: null,
    order: null,
    panelId: null,
    chart: null,
    expLab: null,

    popupstyle: {'background': '#C3C3CB', 'opacity': 0.6, 'padding': 2},

    plot: function () {
    },
    plotUpdate: function () {
    },
    initData: function () {
    },

    //constructor: function (config) {
    //    this.callParent(arguments);
    //},

    initComponent: function () {
        this.callParent(arguments);
        this.initMask();
        Ext.apply(this, this.initialConfig);
        this.title = "";
        this.store && (this.bindStore(this.store, true));
    },

    afterRender: function () {
        this.callParent(arguments);
        this.panelId = '#' + this.id + '-innerCt';
        this.bindComponent(true);
        this.init();
    },

    initMask: function () {
        if (this.loadMask !== false) {
            if (this.loadMask === true) {
                this.loadMask = new Ext.LoadMask({target: this, msg: this.loadMaskMsg});
            }
        }
    },
    showMask: function () {
        if (this.loadMask !== false && this.loadMask !== true && this.rendered && !this.loadMask.isVisible()) {
            this.loadMask.show();
        }
    },
    hideMask: function () {
        if (this.loadMask !== false && this.loadMask !== true && this.rendered && this.loadMask.isVisible()) {
            this.loadMask.hide();
        }
    },

    update: function () {
        if (!this.__initialized)
            return;
        this.d3CanvasUpdate();
        try {
            this.plotUpdate();
        } catch (err) {
            console.log(err);
        }
    },

    init: function () {
        var _this = this;

        this.showMask();
        try {
            this.initData();
        } catch (err) {
            console.log(err);
        }

        var cdelay = 100;
        if (!_this.updateTask) {
            _this.updateTask = new Ext.util.DelayedTask(_this.draw, _this, [], false);
        }
        _this.updateTask.delay(cdelay);
    },

    draw: function () {
        var _this = this;

        if (( (this.store && !this.store.lastOptions) && !this.data) || !_this.rendered) {
            return;
        }
        this.showMask();

        if (_this.chart) {
            console.log("drop chart");
            _this.chart.remove("svg");
        }

        this.d3CanvasInit();
        try {
            this.plot();
        } catch (err) {
            console.log(err);
        }

        this.hideMask();
        this.__initialized = true;
    },

    save: function () {
        var svgstr;
        var _this = this;
        if (!this.chart) return "";
        if (typeof XMLSerializer != "undefined") {
            svgstr = (new XMLSerializer()).serializeToString(this.chart[0][0]);
        } else {
            svgstr = $(this.chart).html();
        }
        return svgstr;
    },

    saveLocal: function () {
        var html = this.save();
        var p = Ext.create('Ext.form.Panel', {
            standardSubmit: true,
            url: 'data/svg.php',
            hidden: true,
            items: [
                {xtype: 'hiddenfield', name: 'id', value: this.plotTitle},
                {xtype: 'hiddenfield', name: 'type', value: "image/svg+xml"},
                {xtype: 'hiddenfield', name: 'svg', value: html}
            ]
        });
        p.getForm().submit
        ({
             success: function (form, action) {
                 p.destroy();
             }
         });
    },

    d3CanvasUpdate: function () {
        this.chart
                .attr('width', this.getWidth())
                .attr('height', this.getHeight());
    },

    d3CanvasInit: function () {

        this.chart = d3.select(this.panelId)
                .append('svg')
                .attr('id','#' + this.id + '-svg')
                .attr('xmlns', 'http://www.w3.org/2000/svg')
                .attr('width', this.getWidth())
                .attr('height', this.getHeight())
                .attr('title', this.plotTitle)
                .attr('version', 1.1)
                .style('position', 'absolute')
                .style('top', 0)
                .style('left', 0);
    },

    tooltiphide: function () {
        this.chart.selectAll('.d3svg-tooltip').remove();
        //svg.selectAll('polygon').remove();
    },
    tooltipmove: function (x, y) {

        this.chart
                .selectAll('.d3svg-tooltip')
                .attr({
                          'x': this.tooltipX(x),
                          'y': this.tooltipY(y)
                      });
    },
    tooltipY: function (y) {
        var height=290;
        if(this.foHeight)
            height=this.foHeight;
        if(y+height > this.getHeight()) {
            return y-height;
        }
        return y;
    },
    tooltipX: function (x) {
        var width=290;
        if(this.foWidth)
            width=this.foWidth;
        if(x+width > this.getWidth()) {
            return x-width;
        }
        return x;
    },
    tooltip: function (x, y, html, foWidth) {
        if (!foWidth)
            foWidth = 160;
        this.foWidth=foWidth;
        var fo = this.chart.append('foreignObject')
                .attr({
                          'x': this.tooltipX(x),
                          'y': this.tooltipY(y),
                          'width': foWidth,
                          'class': 'd3svg-tooltip'
                      });
        var div = fo.append('xhtml:div')
                .style(this.popupstyle)
                .style('height', 'auto')
                .attr({
                          'class': 'd3tooltip'
                      });
        div.html(html);
        var foHeight = div[0][0].getBoundingClientRect().height;
        fo.attr({
                    'height': foHeight
                });
        this.foHeight=foHeight;

        //this.chart.insert('polygon', '.d3svg-tooltip')
        //        .attr({
        //                  'points': "0,0 0," + foHeight + " " + foWidth + "," + foHeight + " " + foWidth + ",0 " + (t) + ",0 " + tip.w + "," + (-tip.h) + " " + (t/2) + ",0",
        //                  'height': foHeight + tip.h,
        //                  'width': foWidth,
        //                  'fill': '#D8D8D8',
        //                  'opacity': 0.75,
        //                  'transform': 'translate(' + (anchor.w - tip.w) + ',' + (anchor.h + tip.h) + ')'
        //              });
    },
    onMove: function () {

    },

    bindStore: function (store, initial) {

        if (!initial && this.store) {
            if (store !== this.store && this.store.autoDestroy) {
                this.store.destroy();
            } else {
                ////                this.store.un("datachanged", this.onDataChange, this);
                //this.store.un("load", this.onLoad, this);
                ////                this.store.un("add", this.onAdd, this);
                //this.store.un("remove", this.onRemove, this);
                //this.store.un("update", this.onUpdate, this);
                ////                this.store.un("clear", this.onClear, this);
            }
        }
        if (store) {
            store = Ext.data.StoreManager.lookup(store);
            this.initialConfig.store = store;
            store.on({
                         scope: this,
                         load: this.onLoad,
                         remove: this.onRemove,
                         update: this.onUpdate,
                         //                         datachanged: this.onDataChange,
                         //                         add: this.onAdd,
                         //                         clear: this.onClear
                     });
        }

        this.store = store;
    },

    destroy: function () {
        var _this = this;

        if (this.expLab) {
            this.expLab.remove();
        }
        if (this.__initialized || this.chart) {
            d3.select(this.panelId).remove();
        }

        this.bindComponent(null);
        this.bindStore(null);
        this.callParent(arguments);
    },

    bindComponent: function (bind) {
        if (bind) {
            //            this.on('move', this.onMove);
            this.on('resize', this._onResize);
        } else {
            //            this.un('move', this.onMove);
            this.un('resize', this._onResize);
        }
    },

    _onResize: function () {
        this.update();
    },

    onLoad: function () {
        var _this = this;
        this.storeLoaded = true;
        this.update();
    },

    onRemove: function () {
        console.log("d3 on remove");
        this.storeLoaded = false;
    },
    onUpdate: function () {
        console.log("d3 on update");
        this.storeLoaded = false;
    },

});

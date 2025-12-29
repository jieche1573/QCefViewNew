import{u as tt,b as et}from"./vue-router-3d8696d7.js";import{f as ut,S as at,O as w}from"./@element-plus-7663beb5.js";import{a as R,_ as ot}from"./index-4c0b7a77.js";import{i as $}from"./echarts-d189ec4a.js";import{u as A,w as rt}from"./xlsx-e2da696d.js";import{E as lt}from"./jspdf-f0fc5da5.js";import it from"./html2canvas-cf611ee3.js";import{E as h}from"./element-plus-49062de6.js";import{d as nt,e as F,m as st,U as dt,c as pt,a as l,R as u,J as o,P as ct,u as _,n as q,al as m,o as Ft,O as k}from"./@vue-dd0bd8be.js";import"./lodash-es-6e490cb3.js";import"./@vueuse-152abd19.js";import"./@popperjs-065daa60.js";import"./@ctrl-d7e2671d.js";import"./dayjs-b0869e06.js";import"./core-js-49dac297.js";import"./async-validator-46a5c7c8.js";import"./memoize-one-5f5358c6.js";import"./normalize-wheel-es-a8f75fb2.js";import"./@floating-ui-68942d04.js";import"./@babel-7c65a846.js";import"./fflate-cf540d17.js";import"./fast-png-1c277078.js";import"./iobuffer-3c2a1e1d.js";import"./pako-8e1dbc52.js";import"./zrender-f1f092e3.js";const ht={class:"evaluation-page"},mt={class:"header"},xt={class:"header-left"},vt={class:"header-right"},ft={class:"selected-count"},gt={class:"main-content"},At={class:"export-dialog-content"},bt={class:"export-option"},Et={class:"export-option"},Bt={class:"export-option"},_t={class:"export-option"},Ct=nt({__name:"Evaluation",setup(Dt){const O=tt(),M=et(),B=F([]),S=F(null),z=F(null),L=F(null),U=F(null),p=F([]),x=F([]),v=F([]),f=F([]),C=F(!1),E=F("excel"),y={},P=()=>{O.back()},H=()=>{const r=R();if(!(r!=null&&r.queryScenarioByIdReq)){console.warn("\u26A0\uFE0F Qt\u5BA2\u6237\u7AEF\u672A\u5C31\u7EEA\uFF0C\u4F7F\u7528\u6A21\u62DF\u6570\u636E"),N();return}B.value.forEach((t,e)=>{r.queryScenarioByIdReq(t)})},V=r=>{try{if(!r){console.warn("\u26A0\uFE0F \u8FD4\u56DE\u6570\u636E\u4E3A\u7A7A");return}const t=JSON.parse(r);console.log("\u{1F4CA} \u63A5\u6536\u5230\u60F3\u5B9A\u6570\u636E:",t);const e=p.value.length+1,n=t.jc||`\u60F3\u5B9A${e}`;p.value.push({index:e,airport:n,rate:`${(t.ryzddlyl*100).toFixed(1)}%`}),x.value.push({index:e,airport:n,rate:`${(t.rymzl*100).toFixed(1)}%`}),v.value.push({index:e,airport:n,rate:`${(t.zbzddlyl*100).toFixed(1)}%`}),f.value.push({index:e,airport:n,rate:`${(t.zbmzl*100).toFixed(1)}%`}),p.value.length===B.value.length&&q(()=>{I()})}catch(t){console.error("\u274C \u89E3\u6790\u60F3\u5B9A\u6570\u636E\u5931\u8D25:",t)}},N=()=>{p.value=[{index:1,airport:"\u673A\u573A1",rate:"50.0%"},{index:2,airport:"\u673A\u573A2",rate:"60.0%"},{index:3,airport:"\u673A\u573A3",rate:"40.0%"},{index:4,airport:"\u673A\u573A4",rate:"20.0%"}],x.value=[{index:1,airport:"\u673A\u573A1",rate:"50.0%"},{index:2,airport:"\u673A\u573A2",rate:"60.0%"},{index:3,airport:"\u673A\u573A3",rate:"40.0%"},{index:4,airport:"\u673A\u573A4",rate:"20.0%"}],v.value=[{index:1,airport:"\u673A\u573A1",rate:"50.0%"},{index:2,airport:"\u673A\u573A2",rate:"60.0%"},{index:3,airport:"\u673A\u573A3",rate:"40.0%"},{index:4,airport:"\u673A\u573A4",rate:"20.0%"}],f.value=[{index:1,airport:"\u673A\u573A1",rate:"50.0%"},{index:2,airport:"\u673A\u573A2",rate:"60.0%"},{index:3,airport:"\u673A\u573A3",rate:"40.0%"},{index:4,airport:"\u673A\u573A4",rate:"20.0%"}],q(()=>{I()})},I=()=>{const r=t=>parseFloat(t.replace("%",""))/100;S.value&&$(S.value).setOption({grid:{left:"15%",right:"10%",top:"15%",bottom:"15%"},xAxis:{type:"category",data:p.value.map(e=>e.airport),axisLabel:{color:"#FFFFFF"}},yAxis:{type:"value",max:1,axisLabel:{color:"#FFFFFF",formatter:e=>`${(e*100).toFixed(0)}%`}},series:[{data:p.value.map(e=>r(e.rate)),type:"bar",itemStyle:{color:"#5DADE2"},label:{show:!0,position:"top",color:"#FFFFFF",formatter:e=>p.value[e.dataIndex].rate}}]}),z.value&&$(z.value).setOption({grid:{left:"15%",right:"10%",top:"15%",bottom:"15%"},xAxis:{type:"category",data:x.value.map(e=>e.airport),axisLabel:{color:"#FFFFFF"}},yAxis:{type:"value",max:1,axisLabel:{color:"#FFFFFF",formatter:e=>`${(e*100).toFixed(0)}%`}},series:[{data:x.value.map(e=>r(e.rate)),type:"bar",itemStyle:{color:"#5DADE2"},label:{show:!0,position:"top",color:"#FFFFFF",formatter:e=>x.value[e.dataIndex].rate}}]}),L.value&&$(L.value).setOption({grid:{left:"15%",right:"10%",top:"15%",bottom:"15%"},xAxis:{type:"category",data:v.value.map(e=>e.airport),axisLabel:{color:"#FFFFFF"}},yAxis:{type:"value",max:1,axisLabel:{color:"#FFFFFF",formatter:e=>`${(e*100).toFixed(0)}%`}},series:[{data:v.value.map(e=>r(e.rate)),type:"bar",itemStyle:{color:"#5DADE2"},label:{show:!0,position:"top",color:"#FFFFFF",formatter:e=>v.value[e.dataIndex].rate}}]}),U.value&&$(U.value).setOption({grid:{left:"15%",right:"10%",top:"15%",bottom:"15%"},xAxis:{type:"category",data:f.value.map(e=>e.airport),axisLabel:{color:"#FFFFFF"}},yAxis:{type:"value",max:1,axisLabel:{color:"#FFFFFF",formatter:e=>`${(e*100).toFixed(0)}%`}},series:[{data:f.value.map(e=>r(e.rate)),type:"bar",itemStyle:{color:"#5DADE2"},label:{show:!0,position:"top",color:"#FFFFFF",formatter:e=>f.value[e.dataIndex].rate}}]})},W=()=>{if(p.value.length===0){h.warning("\u6682\u65E0\u6570\u636E\u53EF\u5BFC\u51FA");return}E.value="excel",C.value=!0},Y=async()=>{C.value=!1,E.value==="excel"?G():E.value==="pdf"?await J():E.value==="word"?Q():E.value==="html"&&K()},G=()=>{try{const r=A.book_new(),t=A.json_to_sheet(p.value.map(i=>({\u5E8F\u53F7:i.index,\u60F3\u5B9A\u540D\u79F0:i.airport,\u4EBA\u5458\u5229\u7528\u7387:i.rate})));A.book_append_sheet(r,t,"\u4EBA\u5458\u5229\u7528\u7387");const e=A.json_to_sheet(x.value.map(i=>({\u5E8F\u53F7:i.index,\u60F3\u5B9A\u540D\u79F0:i.airport,\u4EBA\u5458\u6EE1\u8DB3\u7387:i.rate})));A.book_append_sheet(r,e,"\u4EBA\u5458\u6EE1\u8DB3\u7387");const n=A.json_to_sheet(v.value.map(i=>({\u5E8F\u53F7:i.index,\u60F3\u5B9A\u540D\u79F0:i.airport,\u88C5\u5907\u5229\u7528\u7387:i.rate})));A.book_append_sheet(r,n,"\u88C5\u5907\u5229\u7528\u7387");const a=A.json_to_sheet(f.value.map(i=>({\u5E8F\u53F7:i.index,\u60F3\u5B9A\u540D\u79F0:i.airport,\u88C5\u5907\u6EE1\u8DB3\u7387:i.rate})));A.book_append_sheet(r,a,"\u88C5\u5907\u6EE1\u8DB3\u7387"),rt(r,`\u6548\u679C\u8BC4\u4F30\u5BF9\u6BD4_${Date.now()}.xlsx`),h.success("\u5BFC\u51FAExcel\u6210\u529F")}catch(r){console.error("\u5BFC\u51FAExcel\u5931\u8D25:",r),h.error("\u5BFC\u51FAExcel\u5931\u8D25")}},J=async()=>{try{h.info("\u6B63\u5728\u751F\u6210PDF\uFF0C\u8BF7\u7A0D\u5019...");const r=document.querySelector(".main-content");if(!r)return;const t=await it(r,{backgroundColor:"#2A3348",scale:2}),e=new lt("p","mm","a4"),n=e.internal.pageSize.getWidth(),a=e.internal.pageSize.getHeight(),i=10;e.setFontSize(16),e.text("Effect Evaluation Comparison Report",n/2,15,{align:"center"}),e.setFontSize(10),e.text(`Generated: ${new Date().toLocaleString()}`,n/2,22,{align:"center"});const c=t.toDataURL("image/png"),s=n-2*i,b=t.height*s/t.width;if(b<=a-30)e.addImage(c,"PNG",i,30,s,b);else{let D=30;const j=Math.ceil(b/(a-30));for(let g=0;g<j;g++){g>0&&e.addPage();const d=document.createElement("canvas");d.width=t.width,d.height=Math.min((a-(g===0?30:i))*t.width/s,t.height-g*((a-30)*t.width)/s);const T=d.getContext("2d");if(T){T.drawImage(t,0,g*((a-30)*t.width)/s,t.width,d.height,0,0,t.width,d.height);const X=d.toDataURL("image/png"),Z=d.height*s/d.width;e.addImage(X,"PNG",i,g===0?30:i,s,Z)}}}e.save(`\u6548\u679C\u8BC4\u4F30\u5BF9\u6BD4_${Date.now()}.pdf`),h.success("\u5BFC\u51FAPDF\u6210\u529F")}catch(r){console.error("\u5BFC\u51FAPDF\u5931\u8D25:",r),h.error("\u5BFC\u51FAPDF\u5931\u8D25")}},Q=()=>{try{let r=`
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>\u6548\u679C\u8BC4\u4F30\u5BF9\u6BD4\u62A5\u544A</title>
  <style>
    body { font-family: Arial, "Microsoft YaHei", sans-serif; margin: 40px; }
    h1 { color: #2c3e50; border-bottom: 3px solid #3498db; padding-bottom: 10px; text-align: center; }
    h2 { color: #34495e; margin-top: 30px; }
    .info { background: #ecf0f1; padding: 15px; margin: 20px 0; }
    table { border-collapse: collapse; width: 100%; margin: 15px 0; }
    th { background: #3498db; color: white; padding: 12px; text-align: left; }
    td { padding: 10px; border: 1px solid #bdc3c7; }
    tr:nth-child(even) { background: #f8f9fa; }
    .section { display: flex; gap: 30px; margin-bottom: 30px; }
    .half { flex: 1; }
  </style>
</head>
<body>
  <h1>\u6548\u679C\u8BC4\u4F30\u5BF9\u6BD4\u62A5\u544A</h1>
  <div class="info">
    <p><strong>\u5BF9\u6BD4\u60F3\u5B9A\u6570\u91CF\uFF1A</strong>${B.value.length}</p>
    <p><strong>\u751F\u6210\u65F6\u95F4\uFF1A</strong>${new Date().toLocaleString()}</p>
  </div>

  <div class="section">
    <div class="half">
      <h2>\u4EBA\u5458\u5229\u7528\u7387</h2>
      <table>
        <tr><th>\u5E8F\u53F7</th><th>\u60F3\u5B9A\u540D\u79F0</th><th>\u5229\u7528\u7387</th></tr>
        ${p.value.map(a=>`<tr><td>${a.index}</td><td>${a.airport}</td><td>${a.rate}</td></tr>`).join("")}
      </table>
    </div>
    <div class="half">
      <h2>\u88C5\u5907\u5229\u7528\u7387</h2>
      <table>
        <tr><th>\u5E8F\u53F7</th><th>\u60F3\u5B9A\u540D\u79F0</th><th>\u5229\u7528\u7387</th></tr>
        ${v.value.map(a=>`<tr><td>${a.index}</td><td>${a.airport}</td><td>${a.rate}</td></tr>`).join("")}
      </table>
    </div>
  </div>

  <div class="section">
    <div class="half">
      <h2>\u4EBA\u5458\u6EE1\u8DB3\u7387</h2>
      <table>
        <tr><th>\u5E8F\u53F7</th><th>\u60F3\u5B9A\u540D\u79F0</th><th>\u6EE1\u8DB3\u7387</th></tr>
        ${x.value.map(a=>`<tr><td>${a.index}</td><td>${a.airport}</td><td>${a.rate}</td></tr>`).join("")}
      </table>
    </div>
    <div class="half">
      <h2>\u88C5\u5907\u6EE1\u8DB3\u7387</h2>
      <table>
        <tr><th>\u5E8F\u53F7</th><th>\u60F3\u5B9A\u540D\u79F0</th><th>\u6EE1\u8DB3\u7387</th></tr>
        ${f.value.map(a=>`<tr><td>${a.index}</td><td>${a.airport}</td><td>${a.rate}</td></tr>`).join("")}
      </table>
    </div>
  </div>
</body>
</html>`;const t=new Blob([r],{type:"application/msword;charset=utf-8"}),e=URL.createObjectURL(t),n=document.createElement("a");n.href=e,n.download=`\u6548\u679C\u8BC4\u4F30\u5BF9\u6BD4_${Date.now()}.doc`,n.click(),URL.revokeObjectURL(e),h.success("\u5BFC\u51FAWord\u6210\u529F")}catch(r){console.error("\u5BFC\u51FAWord\u5931\u8D25:",r),h.error("\u5BFC\u51FAWord\u5931\u8D25")}},K=()=>{try{let r=`
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>\u6548\u679C\u8BC4\u4F30\u5BF9\u6BD4\u62A5\u544A</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", "Microsoft YaHei", sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      padding: 20px;
    }
    .container {
      max-width: 1200px;
      margin: 0 auto;
      background: white;
      border-radius: 10px;
      box-shadow: 0 10px 40px rgba(0,0,0,0.2);
      overflow: hidden;
    }
    .header {
      background: linear-gradient(135deg, #3498db 0%, #2980b9 100%);
      color: white;
      padding: 40px;
      text-align: center;
    }
    .header h1 { font-size: 32px; margin-bottom: 10px; }
    .header p { opacity: 0.9; }
    .content { padding: 40px; }
    .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 30px; }
    .card {
      background: #f8f9fa;
      border-radius: 8px;
      padding: 20px;
      border-left: 4px solid #3498db;
    }
    .card h3 { color: #2c3e50; margin-bottom: 15px; }
    table { width: 100%; border-collapse: collapse; }
    th { background: #3498db; color: white; padding: 12px; text-align: left; }
    td { padding: 10px; border-bottom: 1px solid #e0e0e0; }
    tr:hover { background: #f0f0f0; }
    .footer {
      text-align: center;
      padding: 20px;
      background: #2c3e50;
      color: white;
    }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <h1>\u6548\u679C\u8BC4\u4F30\u5BF9\u6BD4\u62A5\u544A</h1>
      <p>\u5BF9\u6BD4\u60F3\u5B9A\u6570\u91CF\uFF1A${B.value.length} | \u751F\u6210\u65F6\u95F4\uFF1A${new Date().toLocaleString()}</p>
    </div>
    <div class="content">
      <div class="grid">
        <div class="card">
          <h3>\u{1F465} \u4EBA\u5458\u5229\u7528\u7387</h3>
          <table>
            <tr><th>\u5E8F\u53F7</th><th>\u60F3\u5B9A\u540D\u79F0</th><th>\u5229\u7528\u7387</th></tr>
            ${p.value.map(a=>`<tr><td>${a.index}</td><td>${a.airport}</td><td><strong>${a.rate}</strong></td></tr>`).join("")}
          </table>
        </div>
        <div class="card">
          <h3>\u{1F697} \u88C5\u5907\u5229\u7528\u7387</h3>
          <table>
            <tr><th>\u5E8F\u53F7</th><th>\u60F3\u5B9A\u540D\u79F0</th><th>\u5229\u7528\u7387</th></tr>
            ${v.value.map(a=>`<tr><td>${a.index}</td><td>${a.airport}</td><td><strong>${a.rate}</strong></td></tr>`).join("")}
          </table>
        </div>
        <div class="card">
          <h3>\u2705 \u4EBA\u5458\u6EE1\u8DB3\u7387</h3>
          <table>
            <tr><th>\u5E8F\u53F7</th><th>\u60F3\u5B9A\u540D\u79F0</th><th>\u6EE1\u8DB3\u7387</th></tr>
            ${x.value.map(a=>`<tr><td>${a.index}</td><td>${a.airport}</td><td><strong>${a.rate}</strong></td></tr>`).join("")}
          </table>
        </div>
        <div class="card">
          <h3>\u2705 \u88C5\u5907\u6EE1\u8DB3\u7387</h3>
          <table>
            <tr><th>\u5E8F\u53F7</th><th>\u60F3\u5B9A\u540D\u79F0</th><th>\u6EE1\u8DB3\u7387</th></tr>
            ${f.value.map(a=>`<tr><td>${a.index}</td><td>${a.airport}</td><td><strong>${a.rate}</strong></td></tr>`).join("")}
          </table>
        </div>
      </div>
    </div>
    <div class="footer">
      \xA9 ${new Date().getFullYear()} \u6548\u679C\u8BC4\u4F30\u7CFB\u7EDF | \u751F\u6210\u65F6\u95F4\uFF1A${new Date().toLocaleString()}
    </div>
  </div>
</body>
</html>`;const t=new Blob([r],{type:"text/html;charset=utf-8"}),e=URL.createObjectURL(t),n=document.createElement("a");n.href=e,n.download=`\u6548\u679C\u8BC4\u4F30\u5BF9\u6BD4_${Date.now()}.html`,n.click(),URL.revokeObjectURL(e),h.success("\u5BFC\u51FAHTML\u6210\u529F")}catch(r){console.error("\u5BFC\u51FAHTML\u5931\u8D25:",r),h.error("\u5BFC\u51FAHTML\u5931\u8D25")}};return st(()=>{const r=M.query.ids;r&&(B.value=r.split(","));const t=R();if(t!=null&&t.queryScenarioById){const e=n=>{V(n)};t.queryScenarioById.connect(e),y.queryScenarioById=e}H()}),dt(()=>{const r=R();!r||Object.keys(y).forEach(t=>{if(y[t]&&r[t])try{r[t].disconnect(y[t])}catch{console.warn(`\u65AD\u5F00\u4FE1\u53F7\u5931\u8D25: ${t}`)}})}),(r,t)=>{const e=m("el-icon"),n=m("el-button"),a=m("el-table-column"),i=m("el-table"),c=m("el-col"),s=m("el-row"),b=m("el-card"),D=m("el-radio"),j=m("el-radio-group"),g=m("el-dialog");return Ft(),pt("div",ht,[l("div",mt,[l("div",xt,[u(n,{onClick:P,size:"large"},{default:o(()=>[u(e,null,{default:o(()=>[u(_(ut))]),_:1}),t[3]||(t[3]=k(" \u8FD4\u56DE ",-1))]),_:1}),t[4]||(t[4]=l("span",{class:"title"},"\u6548\u679C\u8BC4\u4F30\u5BF9\u6BD4",-1))]),l("div",vt,[l("span",ft,"\u5DF2\u9009\u62E9 "+ct(B.value.length)+" \u4E2A\u60F3\u5B9A\u8FDB\u884C\u5BF9\u6BD4",1),u(n,{type:"primary",onClick:W,icon:_(at)},{default:o(()=>[...t[5]||(t[5]=[k(" \u5BFC\u51FA\u7ED3\u679C ",-1)])]),_:1},8,["icon"])])]),l("div",gt,[u(s,{gutter:20},{default:o(()=>[u(c,{span:12},{default:o(()=>[t[8]||(t[8]=l("div",{class:"section-title"},"\u4EBA\u5458\u8BC4\u4F30\u7ED3\u679C",-1)),u(b,{class:"eval-card",shadow:"hover"},{header:o(()=>[...t[6]||(t[6]=[l("div",{class:"card-header"},[l("span",null,"\u4EBA\u5458\u5229\u7528\u7387")],-1)])]),default:o(()=>[u(s,{gutter:20},{default:o(()=>[u(c,{span:12},{default:o(()=>[u(i,{data:p.value,border:"",stripe:"",size:"small"},{default:o(()=>[u(a,{prop:"index",label:"\u5E8F\u53F7",width:"60",align:"center"}),u(a,{prop:"airport",label:"\u673A\u573A",width:"100"}),u(a,{prop:"rate",label:"\u4EBA\u5458\u5229\u7528\u7387",align:"center"})]),_:1},8,["data"])]),_:1}),u(c,{span:12},{default:o(()=>[l("div",{ref_key:"personnelUtilizationChart",ref:S,class:"chart-container"},null,512)]),_:1})]),_:1})]),_:1}),u(b,{class:"eval-card",shadow:"hover",style:{"margin-top":"20px"}},{header:o(()=>[...t[7]||(t[7]=[l("div",{class:"card-header"},[l("span",null,"\u4EBA\u5458\u6EE1\u8DB3\u7387")],-1)])]),default:o(()=>[u(s,{gutter:20},{default:o(()=>[u(c,{span:12},{default:o(()=>[u(i,{data:x.value,border:"",stripe:"",size:"small"},{default:o(()=>[u(a,{prop:"index",label:"\u5E8F\u53F7",width:"60",align:"center"}),u(a,{prop:"airport",label:"\u673A\u573A",width:"100"}),u(a,{prop:"rate",label:"\u4EBA\u5458\u6EE1\u8DB3\u7387",align:"center"})]),_:1},8,["data"])]),_:1}),u(c,{span:12},{default:o(()=>[l("div",{ref_key:"personnelSatisfactionChart",ref:z,class:"chart-container"},null,512)]),_:1})]),_:1})]),_:1})]),_:1}),u(c,{span:12},{default:o(()=>[t[11]||(t[11]=l("div",{class:"section-title"},"\u88C5\u5907\u8BC4\u4F30\u7ED3\u679C",-1)),u(b,{class:"eval-card",shadow:"hover"},{header:o(()=>[...t[9]||(t[9]=[l("div",{class:"card-header"},[l("span",null,"\u88C5\u5907\u5229\u7528\u7387")],-1)])]),default:o(()=>[u(s,{gutter:20},{default:o(()=>[u(c,{span:12},{default:o(()=>[u(i,{data:v.value,border:"",stripe:"",size:"small"},{default:o(()=>[u(a,{prop:"index",label:"\u5E8F\u53F7",width:"60",align:"center"}),u(a,{prop:"airport",label:"\u673A\u573A",width:"100"}),u(a,{prop:"rate",label:"\u88C5\u5907\u5229\u7528\u7387",align:"center"})]),_:1},8,["data"])]),_:1}),u(c,{span:12},{default:o(()=>[l("div",{ref_key:"equipmentUtilizationChart",ref:L,class:"chart-container"},null,512)]),_:1})]),_:1})]),_:1}),u(b,{class:"eval-card",shadow:"hover",style:{"margin-top":"20px"}},{header:o(()=>[...t[10]||(t[10]=[l("div",{class:"card-header"},[l("span",null,"\u88C5\u5907\u6EE1\u8DB3\u7387")],-1)])]),default:o(()=>[u(s,{gutter:20},{default:o(()=>[u(c,{span:12},{default:o(()=>[u(i,{data:f.value,border:"",stripe:"",size:"small"},{default:o(()=>[u(a,{prop:"index",label:"\u5E8F\u53F7",width:"60",align:"center"}),u(a,{prop:"airport",label:"\u673A\u573A",width:"100"}),u(a,{prop:"rate",label:"\u88C5\u5907\u6EE1\u8DB3\u7387",align:"center"})]),_:1},8,["data"])]),_:1}),u(c,{span:12},{default:o(()=>[l("div",{ref_key:"equipmentSatisfactionChart",ref:U,class:"chart-container"},null,512)]),_:1})]),_:1})]),_:1})]),_:1})]),_:1})]),u(g,{modelValue:C.value,"onUpdate:modelValue":t[2]||(t[2]=d=>C.value=d),title:"\u9009\u62E9\u5BFC\u51FA\u683C\u5F0F",width:"500px","close-on-click-modal":!1},{footer:o(()=>[u(n,{onClick:t[1]||(t[1]=d=>C.value=!1)},{default:o(()=>[...t[16]||(t[16]=[k("\u53D6\u6D88",-1)])]),_:1}),u(n,{type:"primary",onClick:Y},{default:o(()=>[...t[17]||(t[17]=[k("\u786E\u8BA4\u5BFC\u51FA",-1)])]),_:1})]),default:o(()=>[l("div",At,[u(j,{modelValue:E.value,"onUpdate:modelValue":t[0]||(t[0]=d=>E.value=d),size:"large"},{default:o(()=>[u(D,{label:"excel",border:""},{default:o(()=>[l("div",bt,[u(e,{size:"24",color:"#67C23A"},{default:o(()=>[u(_(w))]),_:1}),t[12]||(t[12]=l("div",{class:"export-option-text"},[l("div",{class:"export-option-title"},"Excel\u8868\u683C"),l("div",{class:"export-option-desc"},"\u6570\u636E\u8868\u683C\uFF0C\u9002\u5408\u7F16\u8F91\u548C\u5206\u6790")],-1))])]),_:1}),u(D,{label:"pdf",border:""},{default:o(()=>[l("div",Et,[u(e,{size:"24",color:"#F56C6C"},{default:o(()=>[u(_(w))]),_:1}),t[13]||(t[13]=l("div",{class:"export-option-text"},[l("div",{class:"export-option-title"},"PDF\u62A5\u544A"),l("div",{class:"export-option-desc"},"\u5E26\u56FE\u8868\u7684\u5B8C\u6574\u62A5\u544A")],-1))])]),_:1}),u(D,{label:"word",border:""},{default:o(()=>[l("div",Bt,[u(e,{size:"24",color:"#409EFF"},{default:o(()=>[u(_(w))]),_:1}),t[14]||(t[14]=l("div",{class:"export-option-text"},[l("div",{class:"export-option-title"},"Word\u6587\u6863"),l("div",{class:"export-option-desc"},"\u53EF\u7F16\u8F91\u7684\u6587\u6863\u683C\u5F0F")],-1))])]),_:1}),u(D,{label:"html",border:""},{default:o(()=>[l("div",_t,[u(e,{size:"24",color:"#E6A23C"},{default:o(()=>[u(_(w))]),_:1}),t[15]||(t[15]=l("div",{class:"export-option-text"},[l("div",{class:"export-option-title"},"HTML\u7F51\u9875"),l("div",{class:"export-option-desc"},"\u7F51\u9875\u683C\u5F0F\uFF0C\u53EF\u5728\u6D4F\u89C8\u5668\u4E2D\u67E5\u770B")],-1))])]),_:1})]),_:1},8,["modelValue"])])]),_:1},8,["modelValue"])])}}});const Xt=ot(Ct,[["__scopeId","data-v-926df2e5"]]);export{Xt as default};

<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:attribute-set name="s_table">
    <xsl:attribute name="border">1</xsl:attribute>
    <xsl:attribute name="cellpadding">0</xsl:attribute>
    <xsl:attribute name="cellspacing">0</xsl:attribute>
    <xsl:attribute name="style"></xsl:attribute>
</xsl:attribute-set>

<xsl:template match="logic">
    <html>
      <head>
        <style>
          TABLE { border-right: medium none; border-top: medium none; 
               margin: auto auto auto 1in; border-left: medium none;
               border-bottom: medium none; border-collapse: collapse; width: 400pt;}
          TR { page-break-inside: avoid }
          TD { border-right: windowtext 1pt solid; padding-right: 5.4pt; 
                    border-top: windowtext 1pt solid; padding-left: 5.4pt; 
                    font-size: 10pt;  padding-bottom: 0in; border-left: windowtext 1pt solid; 
                    padding-top: 0in; border-bottom: windowtext 1pt solid;
                    font-family: Arial;}
        </style>
      </head> 
      <body>
      <xsl:apply-templates />
      </body>
    </html>
</xsl:template>

<xsl:template match="input">
  <p><b><u>Layout of the input Csv record</u></b> where fieldSeperator : 
  (<b><xsl:value-of select="@fieldSep"/></b>) and recordSeperator : 
  (<b><xsl:value-of select="@recordSep"/></b>).</p>
  <table>
    <thead>        
      <tr>
       <td style="font-weight: bold; width: 28.35pt;  background: #f2f2f2;" valign="top">#</td>
       <td style="font-weight: bold; width: 201.1pt;  background: #f2f2f2;" valign="top">Field Name</td>
       <td style="font-weight: bold; width: 141.75pt; background: #f2f2f2;" valign="top">Data Type</td>
       <td style="font-weight: bold; width: 61.75pt;  background: #f2f2f2;" valign="top">Allow Null</td>
      </tr>
    </thead> 
    <xsl:for-each select="field">
      <tr>
        <td><xsl:number value="position()" format="1" />.</td>
        <td><xsl:value-of select="@name"/></td>
        <td><xsl:value-of select="@type"/>
          <xsl:if test="@precision or @scale">
            (<xsl:value-of select="@precision"/>,<xsl:value-of select="@scale"/>)
          </xsl:if>
        </td>
        <td><xsl:value-of select="@nullable"/></td>
      </tr>
    </xsl:for-each>
  </table>
</xsl:template>

<xsl:template match="extented">
    <p><b><u>Additional self generated fields</u></b>.</p>
    <table>
    <thead>        
      <tr>
       <td style="font-weight: bold; width: 28.35pt;  background: #f2f2f2;" valign="top">#</td>
       <td style="font-weight: bold; width: 201.1pt;  background: #f2f2f2;" valign="top">Field Name</td>
       <td style="font-weight: bold; width: 141.75pt; background: #f2f2f2;" valign="top">Data Type</td>
       <td style="font-weight: bold; width: 61.75pt;  background: #f2f2f2;" valign="top">Allow Null</td>
      </tr>
    </thead> 
    <xsl:for-each select="field">
      <tr>
        <td><xsl:number value="position()" format="1" />.</td>
        <td><xsl:value-of select="@name"/></td>
        <td><xsl:value-of select="@type"/>
          <xsl:if test="@precision or @scale">
            (<xsl:value-of select="@precision"/>,<xsl:value-of select="@scale"/>)
          </xsl:if>
        </td>
        <td><xsl:value-of select="@nullable"/></td>
      </tr>
    </xsl:for-each>
   </table>
</xsl:template>

<xsl:template match="output">
    <p><b><u>Layout of the binary output record</u></b>.</p>
    <table>
    <thead>        
      <tr>
       <td style="font-weight: bold; width: 201.1pt;  background: #f2f2f2;" valign="top">Filename</td>
       <td style="font-weight: bold; width: 201.1pt;  background: #f2f2f2;" valign="top">Field Name</td>
      </tr>
    </thead> 
    <xsl:for-each select="field">
      <tr>
        <td><xsl:value-of select="@name"/>.<xsl:number value="position()-1" format="1" /></td>
        <td><xsl:value-of select="@name"/></td>
      </tr>
    </xsl:for-each>
        </table>
</xsl:template>

<xsl:template match="local">
    <p><b><u>Local variable space</u></b>.</p>
    <table>
    <thead>        
      <tr>
       <td style="font-weight: bold; width: 201.1pt;  background: #f2f2f2;" valign="top">Field Name</td>
       <td style="font-weight: bold; width: 141.75pt; background: #f2f2f2;" valign="top">Data Type</td>
       <td style="font-weight: bold; width: 201.1pt;  background: #f2f2f2;" valign="top">Default Value</td>
      </tr>
    </thead> 
    <xsl:for-each select="field">
      <tr>
        <td><xsl:value-of select="@name"/></td>
        <td><xsl:value-of select="@type"/>
          <xsl:if test="@max and @type='String'">
            (<xsl:value-of select="@max"/>)
          </xsl:if>
          <xsl:if test="not(@max) and @type='String'">
            (100)
          </xsl:if>
        </td>
        <td>
          <xsl:if test="@value">
            <xsl:value-of select="@value"/>
          </xsl:if>
          <xsl:if test="not(@value) or @value=''">
            null
          </xsl:if>
        </td>
      </tr>
    </xsl:for-each>
        </table>
</xsl:template>

<xsl:template match="code">
    <p><b><u>Transformation rules</u></b>.</p>
    <table>
    <tr><td><b>Rule</b></td><td><b>condition</b></td><td><b>Output</b></td><td><b>Function</b></td><td><b>input</b></td></tr>
    <xsl:apply-templates />
    </table>
</xsl:template>

<xsl:template match="rule">
    <xsl:if test="@condition">
      <tr><td><xsl:number value="position()-1" format="1" /></td><td colspan="4"><b>IF</b>(<xsl:value-of select="@condition"/>)</td></tr>
    </xsl:if>
    <xsl:if test="not(@condition)">
      <tr><td colspan="5"><xsl:number value="position()-1" format="1" /></td></tr>
    </xsl:if>
    <xsl:apply-templates />
</xsl:template>

<xsl:template match="function">
    <xsl:if test="@condition">
      <tr><td></td><td>{<b><xsl:value-of select="@condition"/></b>}</td><td>
      <xsl:for-each select="param[@direction='O']">
        <xsl:if test="position() != 1"> , </xsl:if>
        <xsl:value-of select="@name"/>
      </xsl:for-each>
      </td><td>  
      <xsl:value-of select="@name"/> </td><td>
      <xsl:for-each select="param[@direction='I']">
        <xsl:if test="position() != 1"> , </xsl:if>
        <xsl:if test="@storage='Value'"> <xsl:value-of select="text()"/> </xsl:if>
        <xsl:if test="not(@storage='Value')"> <xsl:value-of select="@name"/> </xsl:if>
      </xsl:for-each>
      </td></tr>
    </xsl:if>
    
    <xsl:if test="not(@condition)"> <tr><td></td><td></td><td>
      <xsl:for-each select="param[@direction='O']">
        <xsl:if test="position() != 1"> , </xsl:if>
        <xsl:value-of select="@name"/>
      </xsl:for-each>
      </td><td>  
      <xsl:value-of select="@name"/> </td><td>
      <xsl:for-each select="param[@direction='I']">
        <xsl:if test="position() != 1"> , </xsl:if>
        <xsl:if test="@storage='Value'"> <xsl:value-of select="text()"/> </xsl:if>
        <xsl:if test="not(@storage='Value')"> <xsl:value-of select="@name"/> </xsl:if>
      </xsl:for-each>
      </td></tr>
    </xsl:if>
    
</xsl:template>

</xsl:stylesheet> 

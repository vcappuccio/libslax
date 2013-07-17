<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:template match="configuration">
    <xsl:apply-templates select="system/host-name"/>
  </xsl:template>
  <xsl:template match="host-name">
    <hello>
      <xsl:value-of select="."/>
    </hello>
  </xsl:template>
</xsl:stylesheet>
import React from 'react';
import { Card, Col, Row, Button } from 'antd';
import { Link } from 'react-router-dom';

const { Meta } = Card;

const CandidateList = ({ candidates }) => {
  return (
    <div style={{ padding: 24 }}>
      <h2>筛选后的候选人</h2>
      <Row gutter={16}>
        {candidates.map((candidate, index) => (
          <Col span={8} key={index}>
            <Card
              hoverable
              cover={<img alt={candidate.name} src="https://via.placeholder.com/200" />}
            >
              <Meta
                title={candidate.name}
                description={
                  <>
                    <p><strong>技能:</strong> {candidate.skills.join(', ')}</p>
                    <p><strong>工作经历:</strong> {candidate.experience}</p>
                  </>
                }
              />
              <div style={{ marginTop: 16 }}>
                <Link to={candidate.resumeLink}>
                  <Button type="primary" target="_blank">查看简历</Button>
                </Link>
              </div>
            </Card>
          </Col>
        ))}
      </Row>
    </div>
  );
};

export default CandidateList;
